#include "kdcmotor.h"
//#include "Thorlabs.MotionControl.KCube.DCServo.h"

#include <QtGlobal>
#include <string>

KDCMotor::KDCMotor(long serno)
    : m_serialno(serno)
{

}

long KDCMotor::serialno() const
{
    return m_serialno;
}

void KDCMotor::setSerialno(long newSerialno)
{
    if (m_serialno == newSerialno)
        return;
    m_serialno = newSerialno;
}

bool KDCMotor::status() const
{
    return m_status;
}

void KDCMotor::setStatus(bool newStatus)
{
    if (m_status == newStatus)
        return;
    m_status = newStatus;
}

bool KDCMotor::homed() const
{
    return m_homed;
}

void KDCMotor::setHomed(bool newHomed)
{
    if (m_homed == newHomed)
        return;
    m_homed = newHomed;
}

double KDCMotor::pos() const
{
    return m_pos;
}

void KDCMotor::setPos(double newPos)
{
    if (m_pos == newPos)
        return;
    m_pos = newPos;
}

double KDCMotor::inipos() const
{
    return m_inipos;
}

void KDCMotor::setInipos(double newInipos)
{
    if (m_inipos == newInipos)
        return;
    m_inipos = newInipos;
}


bool KDCMotor::connect()
{
    if (m_status)
        return true;

    setStatus(true);
    return true;
    /*
    // Connect
    if (CC_Open(std::to_string(m_serialno).c_str()) == 0) {
        setStatus(true);
        CC_StartPolling(std::to_string(m_serialno).c_str(), 200);

        return true;
    } else {
        return false;
    }
    */
}

bool KDCMotor::disconnect()
{
    if(!m_status)
        return true;

    /*
    // Stop Polling
    CC_StopPolling(std::to_string(m_serialno).c_str());
    // Close
    CC_Close(std::to_string(m_serialno).c_str());
    */

    setStatus(false);

    return true;
}

bool KDCMotor::goHome()
{
    if(!m_status)
        return false;

    if(m_homed)
        return true;

    // Clear Message Queue
//    CC_ClearMessageQueue(std::to_string(m_serialno).c_str());
//    if (CC_CanHome(std::to_string(m_serialno).c_str())) {
//        CC_Home(std::to_string(m_serialno).c_str());
        getPos();
        return true;
//    } else {
//        return false;
//    }
}

void KDCMotor::movePos(double pos)
{
    if(!m_status)
        return;

//    if (CC_NeedsHoming(std::to_string(m_serialno).c_str())) {
        goHome();
//    }
    moveDpos(toDevicePos(pos));
    getPos();
}

void KDCMotor::moveDpos(int dpos)
{
    if(!m_status)
        return;
//    CC_MoveToPosition(std::to_string(m_serialno).c_str(), dpos);
}

void KDCMotor::getPos()
{
    /* Find the current position of motor */
    if(!m_status)
        return;
    int pos = 0; //CC_GetPosition(std::to_string(m_serialno).c_str());

    setDpos(pos);
    setPos(toRealPos(m_dpos));
}

void KDCMotor::jog(bool flag)
{
    if(!m_status)
        return;

//    if(flag)
//        CC_MoveJog(std::to_string(m_serialno).c_str(), MOT_Forwards);
//    else
//        CC_MoveJog(std::to_string(m_serialno).c_str(), MOT_Backwards);

}

void KDCMotor::findVelocity()
{
    if(!m_status)
        return;

//    CC_GetVelParams(std::to_string(m_serialno).c_str(), &m_dacceleration, &m_dvelocity);

    setVelocity(toRealPos(m_dvelocity));
    setAcceleration(toRealPos(m_dacceleration));
}

void KDCMotor::changeVelocity(double vel, double acc)
{
    if(!m_status)
        return;

    changeDvelocity(toDevicePos(vel), toDevicePos(acc));
}

void KDCMotor::changeDvelocity(int vel, int acc)
{
    if(!m_status)
        return;

//    CC_SetVelParams(std::to_string(m_serialno).c_str(), acc, vel);
}

void KDCMotor::changeJogMode(double stepsize, bool flag)
{
    if(!m_status)
        return;

//    if(flag)
//        CC_SetJogMode(std::to_string(m_serialno).c_str(), MOT_SingleStep, MOT_Immediate);
//    else
//        CC_SetJogMode(std::to_string(m_serialno).c_str(), MOT_Continuous, MOT_Immediate);

    changeDjogMode(toDevicePos(stepsize));

    setJogstep(stepsize);
}

void KDCMotor::changeDjogMode(int stepsize)
{
    if(!m_status)
        return;

//    CC_SetJogStepSize(std::to_string(m_serialno).c_str(), stepsize);
    setDjogstep(stepsize);
}

void KDCMotor::findJogStep()
{
    if(!m_status)
        return;

//    setDjogstep(CC_GetJogStepSize(std::to_string(m_serialno).c_str()));
//    setJogstep(toRealPos(CC_GetJogStepSize(std::to_string(m_serialno).c_str())));
}

double KDCMotor::toRealPos(int val)
{
    return (double)val / StepsPerRev / GearboxRatio * Pitch;
}

int KDCMotor::toDevicePos(double val)
{
    return val / Pitch * StepsPerRev * GearboxRatio;
}

double KDCMotor::velocity() const
{
    return m_velocity;
}

void KDCMotor::setVelocity(double newVelocity)
{
    if (qFuzzyCompare(m_velocity, newVelocity))
        return;
    m_velocity = newVelocity;
}

double KDCMotor::acceleration() const
{
    return m_acceleration;
}

void KDCMotor::setAcceleration(double newAcceleration)
{
    if (qFuzzyCompare(m_acceleration, newAcceleration))
        return;
    m_acceleration = newAcceleration;
}

int KDCMotor::dpos() const
{
    return m_dpos;
}

void KDCMotor::setDpos(int newDpos)
{
    if (m_dpos == newDpos)
        return;
    m_dpos = newDpos;
}

int KDCMotor::dvelocity() const
{
    return m_dvelocity;
}

void KDCMotor::setDvelocity(int newDvelocity)
{
    if (m_dvelocity == newDvelocity)
        return;
    m_dvelocity = newDvelocity;
}

int KDCMotor::dacceleration() const
{
    return m_dacceleration;
}

void KDCMotor::setDacceleration(int newDacceleration)
{
    if (m_dacceleration == newDacceleration)
        return;
    m_dacceleration = newDacceleration;
}

double KDCMotor::jogstep() const
{
    return m_jogstep;
}

void KDCMotor::setJogstep(double newJogstep)
{
    if (qFuzzyCompare(m_jogstep, newJogstep))
        return;
    m_jogstep = newJogstep;
}

int KDCMotor::djogstep() const
{
    return m_djogstep;
}

void KDCMotor::setDjogstep(int newDjogstep)
{
    if (m_djogstep == newDjogstep)
        return;
    m_djogstep = newDjogstep;
}
