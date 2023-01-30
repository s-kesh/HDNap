#include "mirrorcontrol.h"
#include <QDebug>
#include <QString>
#include <QtConcurrent>
#include <QFuture>

#include "Eigen/Dense"
#include <iostream>
#include <unsupported/Eigen/NonLinearOptimization>
#include <unsupported/Eigen/NumericalDiff>

//#include "Thorlabs.MotionControl.KCube.DCServo.h"

// Generic functor
template<typename _Scalar, int NX = Eigen::Dynamic, int NY = Eigen::Dynamic>
struct Functor
{
typedef _Scalar Scalar;
enum {
    InputsAtCompileTime = NX,
    ValuesAtCompileTime = NY
};
typedef Eigen::Matrix<Scalar,InputsAtCompileTime,1> InputType;
typedef Eigen::Matrix<Scalar,ValuesAtCompileTime,1> ValueType;
typedef Eigen::Matrix<Scalar,ValuesAtCompileTime,InputsAtCompileTime> JacobianType;

int m_inputs, m_values;

Functor() : m_inputs(InputsAtCompileTime), m_values(ValuesAtCompileTime) {}
Functor(int inputs, int values) : m_inputs(inputs), m_values(values) {}

int inputs() const { return m_inputs; }
int values() const { return m_values; }

};

struct myfunctor : Functor<double>
{
myfunctor(void): Functor<double>(3,3) {}
Eigen::Vector3d xyz;
double focus;
double edge;

    int operator()(const Eigen::VectorXd &xval, Eigen::VectorXd &fval) const
    {
        Eigen::Vector3d pos;
        double normal = sqrt(1
                             + (2.0/3.0)
                             * ( pow((xval(0) - xval(1)), 2)
                             + pow((xval(1) - xval(2)), 2)
                             + pow((xval(0) - xval(2)), 2) ) / (edge * edge));

        pos(0) =  (-1 * xval(0) + xval(2)) * (focus / edge) / normal;
        pos(1) = (-2 * xval(1) + xval(0) + xval(2)) * (focus / edge) / sqrt(3.0) / normal;
        pos(2) = (xval(0) + xval(1) + xval(2)) / 3.0 + focus / normal;

        fval = xyz - pos;
        return 0;
   }
};


MirrorControl::MirrorControl(QObject *parent)
    : QObject{parent}
{
    if(true) { //TLI_BuildDeviceList() == 0) {
        // Found Devices

        // No of devices
//        setNoofmotors(TLI_GetDeviceListSize());

        // Get Serial nos
        char serialnos[100];
//        TLI_GetDeviceListByTypeExt(serialnos, 100, 27);

        QString str(serialnos);
        QStringList strlist = str.split(",");

        qDebug() << serialnos;

        for (int i = 0; i < m_noofmotors; ++i) {
            long serno = strlist.at(i).toLong();
            m_serialno.append(serno);
            m_motors.append(KDCMotor(serno));
            m_isConnected.append(false);
            m_isHomed.append(false);
            m_motpos.append(-1.0);
            m_motdpos.append(-1);
            m_modposini.append(0);
            m_xyzpos.append(-1.0);
        }
    }
}

double MirrorControl::focus() const
{
    return m_focus;
}

void MirrorControl::setFocus(double newFocus)
{
    if (qFuzzyCompare(m_focus, newFocus))
        return;
    m_focus = newFocus;
    emit focusChanged();
}


void MirrorControl::settohome()
{
    QList<QFuture<bool>> futures;
    for (int i = 0; i < m_noofmotors; ++i) {
        futures.append(QtConcurrent::run(&KDCMotor::goHome, m_motors.at(i)));
    }

    for (int i = 0; i < m_noofmotors; ++i) {
        bool temp = futures.at(i).result();
        m_isHomed[i] = temp;
    }

    emit isHomedChanged();

    getPos();
}

void MirrorControl::settozero()
{
    for (int i = 0; i < m_noofmotors; ++i) {
        QFuture<void> future = QtConcurrent::run(&KDCMotor::movePos, m_motors.at(i), 0);
    }
}

bool MirrorControl::movetopos(double *pos)
{
//    double motpos[3];
    int motdpos[3];

    getMotorDevicefromPos(motdpos,
                          pos,
                          m_modposini.data());
    bool status = movetomotdpos(motdpos);
//    getMotorfromPos(motpos, pos);
//    bool status = movetomotpos(motpos);

    return status;
}

bool MirrorControl::movetomotpos(double *mpos)
{

    for (int i = 0; i < m_noofmotors; ++i) {
            QFuture<void> future = QtConcurrent::run(&KDCMotor::movePos, m_motors.at(i), mpos[i]);
    }
    return true;
}

bool MirrorControl::movetomotdpos( int *mdpos)
{
    for (int i = 0; i < m_noofmotors; ++i) {
         QFuture<void> future = QtConcurrent::run(&KDCMotor::moveDpos, m_motors.at(i), mdpos[i]);
    }
    return true;
}

void MirrorControl::getPos()
{
    QList<double> pos;
    QList<double> motpos;
    QList<int> motdpos;


    for (int i = 0; i < m_noofmotors; ++i) {
        m_motors[i].getPos();
        motpos.append(m_motors[i].pos());
        motdpos.append(m_motors[i].dpos());
    }

    setMotpos(motpos);
    setMotdpos(motdpos);

    pos.resize(motpos.size());

    getPosfromMotorDevice(pos.data(), motdpos.data(), m_modposini.data());
    setXyzpos(pos);
}

void MirrorControl::setMotorPos()
{
    movetomotpos(m_motpos.data());
}

void MirrorControl::setMotordPos()
{
    qDebug() << m_motdpos;
    movetomotdpos(m_motdpos.data());
}

void MirrorControl::setPos()
{
    movetopos(m_xyzpos.data());
}

void MirrorControl::inimotpos()
{
    movetomotdpos(m_modposini.data());
}

void MirrorControl::getPosfromMotorDevice(double *pos,
                                          int *motdpos,
                                          int *motdpos_ini)
{
    double motorposition[3];

    for (int i = 0; i < 3; ++i) {
        motorposition[i] = m_motors[i].toRealPos(motdpos[i])
                - m_motors[i].toRealPos(motdpos_ini[i]);
    }

    getPosfromMotor(pos, motorposition);
}

void MirrorControl::getMotorDevicefromPos(int *motdpos,
                                          double *pos,
                                          int *motdpos_ini)
{
    double motorposition[3];
    getMotorfromPos(motorposition, pos);

    for (int i = 0; i < 3; ++i) {
        motdpos[i] = m_motors[i].toDevicePos(motorposition[i])
                + motdpos_ini[i];
    }

}

void MirrorControl::getPosfromMotor(double *pos, double *motpos)
{
    double normal = sqrt(1
                         + (2.0/3.0)
                         * ( pow((motpos[0] - motpos[1]), 2)
                         + pow((motpos[1] - motpos[2]), 2)
                         + pow((motpos[0] - motpos[2]), 2) ) / (m_edge * m_edge));

    pos[0] = (-1 * motpos[0] + motpos[2]) * (m_focus / m_edge) / normal;
    pos[1] = (-2 * motpos[1] + motpos[0] + motpos[2]) * (m_focus / m_edge) / sqrt(3.0) / normal;
    pos[2] = (motpos[0] + motpos[1] + motpos[2]) / 3.0 + m_focus / normal;
}

void MirrorControl::getMotorfromPos(double *motpos, double *pos)
{
    Eigen::Matrix3d M;
    Eigen::Vector3d B;

    M << 0, -1, 1, -2, 1, 1, 1, 1, 1;
    B << (m_edge / m_focus) * pos[0], (m_edge / m_focus) * sqrt(3.0) * pos[1], (pos[3] - m_focus) * 3;

    // Linear Solution
    Eigen::VectorXd x(3);
    x = M.partialPivLu().solve(B);

    // Nonlinear Solution

    myfunctor fun;
    fun.xyz << pos[0], pos[1], pos[2];
    fun.focus = m_focus;
    fun.edge = m_edge;

    Eigen::NumericalDiff<myfunctor> numDiff(fun);
    Eigen::LevenbergMarquardt<Eigen::NumericalDiff<myfunctor>, double> lm(numDiff);
    int status = lm.minimize(x);

    motpos[0] = x(0);
    motpos[1] = x(1);
    motpos[2] = x(2);
}

void MirrorControl::connect()
{
    QList<QFuture<bool>> futures;
    for (int i = 0; i < m_noofmotors; ++i) {
        futures.append(QtConcurrent::run(&KDCMotor::connect, m_motors.at(i)));
    }

    for (int i = 0; i < m_noofmotors; ++i) {
        bool temp = futures.at(i).result();
        m_isConnected[i] = temp;
    }
    emit isConnectedChanged();
}

void MirrorControl::disconnect()
{
    QList<QFuture<bool>> futures;
    for (int i = 0; i < m_noofmotors; ++i) {
        futures.append(QtConcurrent::run(&KDCMotor::disconnect, m_motors.at(i)));
    }

    for (int i = 0; i < m_noofmotors; ++i) {
        bool temp = futures.at(i).result();
        m_isConnected[i] = !temp;
    }
    emit isConnectedChanged();
}

short MirrorControl::noofmotors() const
{
    return m_noofmotors;
}

void MirrorControl::setNoofmotors(short newNoofmotors)
{
    if (m_noofmotors == newNoofmotors)
        return;
    m_noofmotors = newNoofmotors;
    emit noofmotorsChanged();
}


const QList<double> &MirrorControl::motpos() const
{
    return m_motpos;
}

void MirrorControl::setMotpos(const QList<double> &newMotpos)
{
    if (m_motpos == newMotpos)
        return;
    m_motpos = newMotpos;
    emit motposChanged();
}

const QList<bool> &MirrorControl::isConnected() const
{
    return m_isConnected;
}

void MirrorControl::setIsConnected(const QList<bool> &newIsConnected)
{
    if (m_isConnected == newIsConnected)
        return;
    m_isConnected = newIsConnected;
    emit isConnectedChanged();
}

const QList<bool> &MirrorControl::isHomed() const
{
    return m_isHomed;
}

void MirrorControl::setIsHomed(const QList<bool> &newIsHomed)
{
    if (m_isHomed == newIsHomed)
        return;
    m_isHomed = newIsHomed;
    emit isHomedChanged();
}

const QList<long> &MirrorControl::serialno() const
{
    return m_serialno;
}

void MirrorControl::setSerialno(const QList<long> &newSerialno)
{
    if (m_serialno == newSerialno)
        return;
    m_serialno = newSerialno;
    emit serialnoChanged();
}

double MirrorControl::edge() const
{
    return m_edge;
}

void MirrorControl::setEdge(double newEdge)
{
    if (qFuzzyCompare(m_edge, newEdge))
        return;
    m_edge = newEdge;
    emit edgeChanged();
}

const QList<double> &MirrorControl::xyzpos() const
{
    return m_xyzpos;
}

void MirrorControl::setXyzpos(const QList<double> &newXyzpos)
{
    if (m_xyzpos == newXyzpos)
        return;
    m_xyzpos = newXyzpos;
    emit xyzposChanged();
}

const QList<int> &MirrorControl::motdpos() const
{
    return m_motdpos;
}

void MirrorControl::setMotdpos(const QList<int> &newMotdpos)
{
    if (m_motdpos == newMotdpos)
        return;
    m_motdpos = newMotdpos;
    emit motdposChanged();
}

const QList<int> &MirrorControl::modposini() const
{
    return m_modposini;
}

void MirrorControl::setModposini(const QList<int> &newModposini)
{
    if (m_modposini == newModposini)
        return;
    m_modposini = newModposini;
    emit modposiniChanged();
}
