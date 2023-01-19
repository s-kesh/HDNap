#ifndef KDCMOTOR_H
#define KDCMOTOR_H

/******************************
Z812

General

Name = Z812
Stage ID = 99
Axis ID = SingleAxis

Home Settings

HomeDir = MOT_Reverse
HomeLimitSwitch = MOT_ReverseLimitSwitch
HomeVel = 1.0
HomeZeroOffset = 0.3

Jog Settings

JogMode = MOT_ExtSingleStep
JogStopMode = MOT_Profiled
JogStepSize = 0.1
JogMinVel = 0.0
JogAccn = 2.0
JogMaxVel = 2.0

Control Settings

DefMinVel = 0.0
DefAccn = 1.5
DefMaxVel = 2.3

Limit Settings

CWHardLimit = MOT_LimitSwitchMakeOnContact
CCWHardLimit = MOT_LimitSwitchMakeOnContact
CWSoftLimit = 3.0
CCWSoftLimit = 1.0
SoftLimitMode = MOT_LimitSwitchIgnored
LimitsSoftwareApproachPolicy = DisallowIllegalMoves

Physical Settings

Pitch = 1.0
StepsPerRev = 512
GearboxRatio = 67.49016
TST101 Stage ID = 0
UseDeviceUnits = False
Units = MOT_Linear
DirSense = MOT_Backwards
MinPos = 0.00
MaxPos = 12.00
MaxAccn = 4.0
MaxVel = 2.6
Factor = 1.0
UnitsTxt = mm
DisplayDP = -1

Misc. Settings

BacklashDist = 0.025
MoveFactor = 100
RestFactor = 20

MMI Settings

WheelMode = KMOT_WM_Jog
WheelMaxVelocity = 1
WheelAcceleration = 2
DirectionSense = KMOT_WM_Positive
PresetPosition1 = 0
PresetPosition2 = 0
DisplayIntensity = 60
MMI_Params.DisplayTimeout = 10
MMI_Params.DisplayDimIntensity = 2

Trigger Config Settings

Trigger1Mode = KMOT_TrigDisabled
Trigger1Polarity = KMOT_TrigPolarityHigh
Trigger2Mode = KMOT_TrigDisabled
Trigger2Polarity = KMOT_TrigPolarityHigh

Trigger Params Settings

TriggerStartPositionFwd = 0
TriggerIntervalFwd = 0
TriggerPulseCountFwd = 1
TriggerStartPositionRev = 0
TriggerIntervalRev = 0
TriggerPulseCountRev = 1
TriggerPulseWidth = 50000
CycleCount = 1
AbsoluteMovePosition = 0
RelativeMovePosition = 1

DC Motor Settings

Pitch = 1.0
StepsPerRev = 512
GearboxRatio = 67.49016
TST101 Stage ID = 0
UseDeviceUnits = False
Units = MOT_Linear
DirSense = MOT_Backwards
MinPos = 0.00
MaxPos = 12.00
MaxAccn = 4.0
MaxVel = 2.6
Factor = 1.0
UnitsTxt = mm
DisplayDP = -1

DC Servo Settings

DCServo = 1
DCProp = 435
DCInt = 195
DCDiff = 993
DCIntLim = 195
************/

/*************
Z812B

General

Name = Z812B
Stage ID = 99
Axis ID = SingleAxis

Home Settings

HomeDir = MOT_Reverse
HomeLimitSwitch = MOT_ReverseLimitSwitch
HomeVel = 1.0
HomeZeroOffset = 0.3

Jog Settings

JogMode = MOT_ExtSingleStep
JogStopMode = MOT_Profiled
JogStepSize = 0.1
JogMinVel = 0.0
JogAccn = 2.0
JogMaxVel = 2.0

Control Settings

DefMinVel = 0.0
DefAccn = 1.5
DefMaxVel = 2.3

Limit Settings

CWHardLimit = MOT_LimitSwitchMakeOnContact
CCWHardLimit = MOT_LimitSwitchMakeOnContact
CWSoftLimit = 3.0
CCWSoftLimit = 1.0
SoftLimitMode = MOT_LimitSwitchIgnored
LimitsSoftwareApproachPolicy = DisallowIllegalMoves

Physical Settings

Pitch = 1.0
StepsPerRev = 512
GearboxRatio = 67.49016
TST101 Stage ID = 0
UseDeviceUnits = False
Units = MOT_Linear
DirSense = MOT_Backwards
MinPos = 0.00
MaxPos = 12.00
MaxAccn = 4.0
MaxVel = 2.6
Factor = 1.0
UnitsTxt = mm
DisplayDP = -1

Misc. Settings

BacklashDist = 0.025
MoveFactor = 100
RestFactor = 20

MMI Settings

WheelMode = KMOT_WM_Jog
WheelMaxVelocity = 1
WheelAcceleration = 2
DirectionSense = KMOT_WM_Positive
PresetPosition1 = 0
PresetPosition2 = 0
DisplayIntensity = 60
MMI_Params.DisplayTimeout = 10
MMI_Params.DisplayDimIntensity = 2

Trigger Config Settings

Trigger1Mode = KMOT_TrigDisabled
Trigger1Polarity = KMOT_TrigPolarityHigh
Trigger2Mode = KMOT_TrigDisabled
Trigger2Polarity = KMOT_TrigPolarityHigh

Trigger Params Settings

TriggerStartPositionFwd = 0
TriggerIntervalFwd = 0
TriggerPulseCountFwd = 1
TriggerStartPositionRev = 0
TriggerIntervalRev = 0
TriggerPulseCountRev = 1
TriggerPulseWidth = 50000
CycleCount = 1
AbsoluteMovePosition = 0
RelativeMovePosition = 1

DC Motor Settings

Pitch = 1.0
StepsPerRev = 512
GearboxRatio = 67.49016
TST101 Stage ID = 0
UseDeviceUnits = False
Units = MOT_Linear
DirSense = MOT_Backwards
MinPos = 0.00
MaxPos = 12.00
MaxAccn = 4.0
MaxVel = 2.6
Factor = 1.0
UnitsTxt = mm
DisplayDP = -1

DC Servo Settings

DCServo = 1
DCProp = 435
DCInt = 195
DCDiff = 993
DCIntLim = 195
***************/



class KDCMotor
{
public:
    explicit KDCMotor(long serno);

    long serialno() const;
    void setSerialno(long newSerialno);
    bool status() const;
    void setStatus(bool newStatus);
    bool homed() const;
    void setHomed(bool newHomed);
    double pos() const;
    void setPos(double newPos);
    double inipos() const;
    void setInipos(double newInipos);
    double velocity() const;
    void setVelocity(double newVelocity);
    double acceleration() const;
    void setAcceleration(double newAcceleration);
    int dpos() const;
    void setDpos(int newDpos);
    int dvelocity() const;
    void setDvelocity(int newDvelocity);
    int dacceleration() const;
    void setDacceleration(int newDacceleration);
    double jogstep() const;
    void setJogstep(double newJogstep);
    int djogstep() const;
    void setDjogstep(int newDjogstep);

    bool connect();
    bool disconnect();
    bool goHome();
    void movePos(double pos);
    void moveDpos(int dpos);
    void getPos();
    void jog(bool flag);
    void findVelocity();
    void changeVelocity(double vel, double acc);
    void changeDvelocity(int vel, int acc);
    void changeJogMode(double stepsize, bool jogflag);
    void changeDjogMode(int stepsize);
    void findJogStep();


    double toRealPos(int val);
    int toDevicePos(double val);


private:
    const double Pitch = 1.0;
    const int StepsPerRev = 512;
    const double GearboxRatio = 67.49016;

    long m_serialno = 0;
    bool m_status = false;
    bool m_homed = false;
    double m_pos = -1;
    double m_inipos = -1;

    double m_velocity;
    double m_acceleration;
    int m_dpos;
    int m_dvelocity;
    int m_dacceleration;
    double m_jogstep;
    int m_djogstep;
};

#endif // KDCMOTOR_H
