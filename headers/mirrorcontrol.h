#ifndef MIRRORCONTROL_H
#define MIRRORCONTROL_H

#include <QObject>
#include <QDateTime>
#include "kdcmotor.h"

class MirrorControl : public QObject
{
    Q_OBJECT

    Q_PROPERTY(short noofmotors READ noofmotors WRITE setNoofmotors NOTIFY noofmotorsChanged)
    Q_PROPERTY(QList<long> serialno READ serialno WRITE setSerialno NOTIFY serialnoChanged)

    Q_PROPERTY(QList<double> xyzpos READ xyzpos WRITE setXyzpos NOTIFY xyzposChanged)

    Q_PROPERTY(double focus READ focus WRITE setFocus NOTIFY focusChanged)
    Q_PROPERTY(double edge READ edge WRITE setEdge NOTIFY edgeChanged)

    Q_PROPERTY(QList<double> motpos READ motpos WRITE setMotpos NOTIFY motposChanged)
    Q_PROPERTY(QList<int> motdpos READ motdpos WRITE setMotdpos NOTIFY motdposChanged)
    Q_PROPERTY(QList<int> modposini READ modposini WRITE setModposini NOTIFY modposiniChanged)

    // Status
    Q_PROPERTY(QList<bool> isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(QList<bool> isHomed READ isHomed WRITE setIsHomed NOTIFY isHomedChanged)

public:
    explicit MirrorControl(QObject *parent = nullptr);

    double focus() const;
    void setFocus(double newFocus);

    short noofmotors() const;
    void setNoofmotors(short newNoofmotors);


    const QList<double> &motpos() const;
    void setMotpos(const QList<double> &newMotpos);

    const QList<bool> &isConnected() const;
    void setIsConnected(const QList<bool> &newIsConnected);

    const QList<bool> &isHomed() const;
    void setIsHomed(const QList<bool> &newIsHomed);

    const QList<long> &serialno() const;
    void setSerialno(const QList<long> &newSerialno);

    double edge() const;
    void setEdge(double newEdge);

    const QList<double> &xyzpos() const;
    void setXyzpos(const QList<double> &newXyzpos);

    const QList<int> &motdpos() const;
    void setMotdpos(const QList<int> &newMotdpos);

public slots:
    void connect();
    void disconnect();
    void settohome();
    void settozero();
    void getPos();
    void setMotorPos();
    void setMotordPos();
    void setPos();
    void inimotpos();

private slots:
    bool movetopos(double *pos);
    bool movetomotpos(double *mpos);
    bool movetomotdpos(int *mdpos);
    void getPosfromMotor(double *pos, double *motpos);
    void getMotorfromPos(double *motpos, double *pos);
    void getPosfromMotorDevice(double *pos,
                               int *motdpos,
                               int *motdpos_ini);
    void getMotorDevicefromPos(int *motdpos,
                               double *pos,
                               int *motdpos_ini);

signals:

    void focusChanged();

    void motorsChanged();
    void noofmotorsChanged();

    void motposChanged();

    void isConnectedChanged();

    void isHomedChanged();

    void serialnoChanged();

    void edgeChanged();

    void xyzposChanged();

    void motdposChanged();

    void modposiniChanged();

public:
    QList<KDCMotor> m_motors;

    const QList<int> &modposini() const;
    void setModposini(const QList<int> &newModposini);

private:
    double m_focus = -1000;
    short m_noofmotors = -1;

    QList<double> m_motpos;
    QList<bool> m_isConnected;
    QList<bool> m_isHomed;
    QList<long> m_serialno;
    double m_edge;
    QList<double> m_xyzpos;
    QList<int> m_motdpos;
    QList<int> m_modposini;
};

#endif // MIRRORCONTROL_H

