#include "camera.h"
#include "global.h"
#include <cstring>
#include <pylon/InstantCamera.h>
#include <iostream>

Camera::Camera(QObject *parent)
    : QObject{parent},
      m_settings(new DevicePropertyModel),
      m_isConnected(false),
      m_isAcquiring(false),
      m_isSaving(false),
      m_index(0),
      m_hitthreshold(1.0),
      m_camera(nullptr),
      imageBuffer(nullptr),
      m_producer(nullptr),
      m_consumer(nullptr),
      m_count(0)
{
    /* Properties of Camera
     * Serial No
     *
     * Auto Gain
     * Auto Exposure
     * Gain (dB)
     * Gamma
     * Exposure (μs)
     *
     * Width (px)
     * Height (px)
     *
     * Trigger Mode
     * Trigger Source
     * Trigger Delay (μs)
     */

    m_settings->addItem("Serial No", "Not Connected", 0, 0, 0, 0);
    m_propmap.insert("Serial No", m_settings->index(0));

    // Syntax to Add Item in model
    // "Name", Value, Minimum, Maximum, StepSize, No of Decimal Points

    // Options Are
    // 0 -> OFF
    // 1 -> Once
    // 2 -> Continues
    m_settings->addItem("Auto Gain", 0, 0, 3, 1, 0);
    m_propmap.insert("Auto Gain", m_settings->index(1));
    m_settings->addItem("Auto Exposure", 1, 0, 3, 1, 0);
    m_propmap.insert("Auto Exposure", m_settings->index(2));

    m_settings->addItem("Gain", 12, 0, 24, 0.1, 1);
    m_propmap.insert("Gain", m_settings->index(3));
    m_settings->addItem("Gamma", 1, 0, 3, 0.00001, 5);
    m_propmap.insert("Gamma", m_settings->index(4));
    m_settings->addItem("Exposure", 10000, 34, 10000000, 1000, 0);
    m_propmap.insert("Exposure", m_settings->index(5));

    m_settings->addItem("Width", 512, 0, 1024, 4, 0);
    m_propmap.insert("Width", m_settings->index(6));
    m_settings->addItem("Height", 512, 0, 1024, 4, 0);
    m_propmap.insert("Height", m_settings->index(7));


    // Options Are
    // 0 -> OFF
    // 1 -> On
    m_settings->addItem("Trigger Mode", 1, 0, 0, 0, 0);
    m_propmap.insert("Trigger Mode", m_settings->index(8));

    // Options Are
    m_settings->addItem("Trigger Source", 0, 0, 0, 0, 0);
    m_propmap.insert("Trigger Source", m_settings->index(9));

    m_settings->addItem("Trigger Delay", 1000, 0, 1000000, 1, 1);
    m_propmap.insert("Trigger Delay", m_settings->index(10));

//    QObject::connect(m_settings, &DevicePropertyModel::valueChanged,
//                     this, &Camera::printNameandValue);

    Pylon::PylonInitialize();

}

Camera::~Camera()
{
    Pylon::PylonTerminate();
}


void Camera::getProperties()
{
    /* Function is used to read properties from the camera
     * Camera need to be connected before this function
     */

    if (!isConnected()) {
        emit warning("Camera is not Connected\n");
        return;
    }

    // Get the camera control object.
    GenApi::INodeMap& nodemap = m_camera->GetNodeMap();

    // Get the parameters for settings.
    Pylon::CEnumParameter lautogain( nodemap, "GainAuto");
    Pylon::CEnumParameter lautoexpo( nodemap, "ExposureAuto");

    Pylon::CFloatParameter lgain( nodemap, "Gain");
    Pylon::CFloatParameter lgamma( nodemap, "Gamma");
    Pylon::CFloatParameter lexpo( nodemap, "ExposureTime");

    Pylon::CIntegerParameter lwidth( nodemap, "Width" );
    Pylon::CIntegerParameter lheight( nodemap, "Height" );

    Pylon::CEnumParameter ltriggermode( nodemap, "TriggerMode");
    Pylon::CEnumParameter ltriggersource( nodemap, "TriggerSource");
    Pylon::CFloatParameter ltriggerdelay( nodemap, "TriggerDelay");

    m_settings->setData(m_propmap["Auto Gain"],
                        QVariant::fromValue(lautogain.GetIntValue()),
                        m_settings->ValueRole);

    m_settings->setData(m_propmap["Auto Exposure"],
                        QVariant::fromValue(lautoexpo.GetIntValue()),
                        m_settings->ValueRole);

    m_settings->setData(m_propmap["Gain"],
                        QVariant::fromValue(lgain.GetValue()),
                        m_settings->ValueRole);
    m_settings->setData(m_propmap["Gain"],
                        QVariant::fromValue(lgain.GetMin()),
                        m_settings->MinRole);
    m_settings->setData(m_propmap["Gain"],
                        QVariant::fromValue(lgain.GetMax()),
                        m_settings->MaxRole);

    m_settings->setData(m_propmap["Gamma"],
                        QVariant::fromValue(lgamma.GetValue()),
                        m_settings->ValueRole);
    m_settings->setData(m_propmap["Gamma"],
                        QVariant::fromValue(lgamma.GetMin()),
                        m_settings->MinRole);
    m_settings->setData(m_propmap["Gamma"],
                        QVariant::fromValue(lgamma.GetMax()),
                        m_settings->MaxRole);

    m_settings->setData(m_propmap["Exposure"],
                        QVariant::fromValue(lexpo.GetValue()),
                        m_settings->ValueRole);
    m_settings->setData(m_propmap["Exposure"],
                        QVariant::fromValue(lexpo.GetMin()),
                        m_settings->MinRole);
    m_settings->setData(m_propmap["Exposure"],
                        QVariant::fromValue(lexpo.GetMax()),
                        m_settings->MaxRole);

    m_settings->setData(m_propmap["Width"],
                        QVariant::fromValue(lwidth.GetValue()),
                        m_settings->ValueRole);
    m_settings->setData(m_propmap["Width"],
                        QVariant::fromValue(lwidth.GetMin()),
                        m_settings->MinRole);
    m_settings->setData(m_propmap["Width"],
                        QVariant::fromValue(lwidth.GetMax()),
                        m_settings->MaxRole);

    m_settings->setData(m_propmap["Height"],
                        QVariant::fromValue(lheight.GetValue()),
                        m_settings->ValueRole);
    m_settings->setData(m_propmap["Height"],
                        QVariant::fromValue(lheight.GetMin()),
                        m_settings->MinRole);
    m_settings->setData(m_propmap["Height"],
                        QVariant::fromValue(lheight.GetMax()),
                        m_settings->MaxRole);

    m_settings->setData(m_propmap["Trigger Mode"],
                        QVariant::fromValue(ltriggermode.GetIntValue()),
                        m_settings->ValueRole);

    m_settings->setData(m_propmap["Trigger Source"],
                        QVariant::fromValue(ltriggersource.GetIntValue()),
                        m_settings->ValueRole);

    m_settings->setData(m_propmap["Trigger Delay"],
                        QVariant::fromValue(ltriggerdelay.GetValue()),
                        m_settings->ValueRole);
    m_settings->setData(m_propmap["Trigger Delay"],
                        QVariant::fromValue(ltriggerdelay.GetMin()),
                        m_settings->MinRole);
    m_settings->setData(m_propmap["Trigger Delay"],
                        QVariant::fromValue(ltriggerdelay.GetMax()),
                        m_settings->MaxRole);
}

void Camera::setProperties()
{
    if (!isConnected()) {
        // It is not connected
        // Issue warning and return
        emit warning("Not Connected\n");
        return;
    }

    if (isAcquiring()) {
        emit warning("It is Acquiring, stop the run and restart\n");
        return;
    }

    // Get the camera control object.
    GenApi::INodeMap& nodemap = m_camera->GetNodeMap();

    // Get the parameters for settings.
    Pylon::CEnumParameter lautogain( nodemap, "GainAuto");
    Pylon::CEnumParameter lautoexpo( nodemap, "ExposureAuto");

    Pylon::CFloatParameter lgain( nodemap, "Gain");
    Pylon::CFloatParameter lgamma( nodemap, "Gamma");
    Pylon::CFloatParameter lexpo( nodemap, "ExposureTime");

    Pylon::CIntegerParameter lwidth( nodemap, "Width" );
    Pylon::CIntegerParameter lheight( nodemap, "Height" );

    Pylon::CEnumParameter ltriggermode( nodemap, "TriggerMode");
    Pylon::CEnumParameter ltriggersource( nodemap, "TriggerSource");
    Pylon::CFloatParameter ltriggerdelay( nodemap, "TriggerDelay");


    int autogain = m_settings->data(m_propmap["Auto Gain"],
                                    m_settings->ValueRole).toInt();
    lautogain.SetIntValue(autogain);

    int autoexpo = m_settings->data(m_propmap["Auto Exposure"],
                                    m_settings->ValueRole).toInt();
    lautoexpo.SetIntValue(autoexpo);

    double gain = m_settings->data(m_propmap["Gain"],
                                   m_settings->ValueRole).toDouble();
    lgain.SetValue(gain);
    double gamma = m_settings->data(m_propmap["Gamma"],
                                    m_settings->ValueRole).toDouble();
    lgamma.SetValue(gamma);
    double expo = m_settings->data(m_propmap["Exposure"],
                                   m_settings->ValueRole).toDouble();
    lexpo.SetValue(expo);

    int width = m_settings->data(m_propmap["Width"],
                                 m_settings->ValueRole).toInt();
    lwidth.SetValue(width);
    int height = m_settings->data(m_propmap["Height"],
                                  m_settings->ValueRole).toInt();
    lheight.SetValue(height);

    int triggermode = m_settings->data(m_propmap["Trigger Mode"],
                                       m_settings->ValueRole).toInt();
    ltriggermode.SetIntValue(triggermode);
    int triggersource = m_settings->data(m_propmap["Trigger Source"],
                                         m_settings->ValueRole).toInt();
    ltriggersource.SetIntValue(triggersource);
    double triggerdelay = m_settings->data(m_propmap["Trigger Delay"],
                                           m_settings->ValueRole).toDouble();
    ltriggerdelay.SetValue(triggerdelay);
}

void Camera::startsession()
{
    /* Function to connect to camera
     * It will find the connected camera
     * And then Connect to it
     * It can take a little time to establise the connection
     */

    if (isConnected()) {
        // It is already connected
        // Issue warning and return
        emit warning("Already Connected\n");
        return;
    }

    try {
        // Attact Camera
        m_camera = new Pylon::CInstantCamera(Pylon::CTlFactory::GetInstance().CreateFirstDevice());
        // Set Serial No
        m_settings->setData(m_propmap["Serial No"],
                            QVariant::fromValue(
                                QString(m_camera->GetDeviceInfo().GetSerialNumber().c_str())),
                            m_settings->ValueRole);
        // Open Camera
        m_camera->Open();
        setIsConnected(true);

        getProperties();
    }
    catch (GenICam::GenericException &e) {
        m_camera = nullptr;
        QString text = "Camera Not able to Connect ";
        text.append(e.GetDescription());
        std::cout << text.toStdString() << std::endl;
        emit error(text);
    }
}

void Camera::stopsession()
{
    if (!isConnected()) {
        // It is already not connected
        // Issue warning and return
        emit warning("Already not Connected\n");
        return;
    }

    if (m_isAcquiring) {
        stopacquiringImages();
    }

    m_camera->Close();
    delete m_camera;

    setIsConnected(false);
}

void Camera::acquireImages(const int count,
                           const QString filename)
{
    if (!isConnected()) {
        // It is not connected
        // Issue warning and return
        emit warning("Not Connected\n");
        return;
    }

    if (isAcquiring()) {
        emit warning("It is Acquiring, stop the run and restart\n");
        return;
    }

    // set Written Values
    setProperties();


    m_width = m_settings->data(m_propmap["Width"],
            m_settings->ValueRole).toInt();
    m_height = m_settings->data(m_propmap["Height"],
            m_settings->ValueRole).toInt();
    emit widthChanged();
    emit heightChanged();

    // Save Important Details in JSON File
    // Save Width, Height of image

    imageBuffer = new LiveImage [BUFFSIZE];
    usedSemp = new QSemaphore;
    freeSemp = new QSemaphore(BUFFSIZE);
    for (int i = 0; i < BUFFSIZE; ++i)
        (imageBuffer + i)->data = new std::uint8_t [m_width * m_height];

    setCount(count);
    setIndex(-1);
    sumImage.clear();
    avgImage.clear();

    // set Max Internal Buffer
    m_camera->MaxNumBuffer = 20;

    // Setup Producer thread
    m_producer = new AcquireCamera(m_camera,
                                   m_count,
                                   usedSemp,
                                   freeSemp,
                                   imageBuffer);

    // Setup Consumer thread
    m_consumer = new LiveImageConsumer(m_count,
                                       m_isSaving,
                                       usedSemp,
                                       freeSemp,
                                       imageBuffer,
                                       filename.toStdString());

    connect(m_consumer,
            &LiveImageConsumer::resultReady,
            this,
            &Camera::handleSignal);

    sumImage.resize(m_width * m_height);
    avgImage.resize(m_width * m_height);


    // Start Consumer thread
    m_consumer->init(1.0,
                     m_width * m_height,
                        sumImage.data(),
                        avgImage.data());
    m_consumer->start();

    // start Grabbing
    m_camera->StartGrabbing(m_count,
                            Pylon::GrabStrategy_OneByOne);

    // start thread
    m_producer->start();

    setIsAcquiring(true);

    m_timer =  new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &Camera::setLreqTrue);
    m_stimer =  new QTimer(this);
    connect(m_stimer, &QTimer::timeout, this, &Camera::setSreqTrue);

    m_timer->start(1000 / m_liverefreshrate);
    m_stimer->start(1000 / m_sumrefreshrate);
}

void Camera::stopacquiringImages()
{
    if (!m_isConnected) {
        // Connect hi nahi hain
        return;
    }

    if (!isAcquiring()) {
        // Already not Acquiring
        emit warning("Nahi ho raha hain Acquire");
        return;
    }

    m_timer->stop();
    m_stimer->stop();
    delete m_timer;
    delete m_stimer;

    if (m_producer->isRunning()) {
        m_producer->stop();
        m_producer->wait();
    }

    if (m_consumer->isRunning()) {
        m_consumer->stop();
        m_consumer->wait();
    }

    m_camera->StopGrabbing();

    delete m_consumer;

    delete m_producer;
    delete usedSemp;
    delete freeSemp;

    for (int i = 0; i < BUFFSIZE; ++i)
        delete [] (imageBuffer + i)->data;

    delete [] imageBuffer;

    setIsAcquiring(false);
}

void Camera::handleSignal(std::uint64_t index)
{
    if (!m_isAcquiring) {
        // Bhaiya Acquire hi nahi kar rahe hain to dekhayege kiya
        return;
    }
    setIndex(index);

    // If hit, copy the result to hit buffer
    if (imageBuffer[index % BUFFSIZE].hit) {
        emit itisaHit(index,
                      m_width, m_height,
                      imageBuffer[index % BUFFSIZE].data);
    }

    // emit the live image
    if (m_lrequested) {
        emit resultReady(index,
                m_width, m_height,
                imageBuffer[index % BUFFSIZE].data);
        setLrequested(false);
    }

    // If sum is asked, emit sum
    // else emit average
    if (m_srequested) {
        if (!m_showSum) {
            emit avgReady(index,
                          m_width, m_height,
                          avgImage.data());
        }
        else {
            emit avgReady(index,
                          m_width, m_height,
                          sumImage.data());
        }
        setSrequested(false);
        m_consumer->setRequested(false);
    }
    if (m_index == m_count - 1)
        stopacquiringImages();
}

void Camera::forceEmit()
{
    if (m_index < 1)
        return;

    if (!m_showSum) {
        emit avgReady(m_index,
                      m_width, m_height,
                      avgImage.data());
    }
    else {
        emit avgReady(m_index,
                      m_width, m_height,
                      sumImage.data());
    }
}

bool Camera::srequested() const
{
    return m_srequested;
}

void Camera::setSrequested(bool newSrequested)
{
    m_srequested = newSrequested;
}

void Camera::setLreqTrue()
{
    setLrequested(true);
}

void Camera::setSreqTrue()
{
    setSrequested(true);
    m_consumer->setRequested(true);
}

bool Camera::lrequested() const
{
    return m_lrequested;
}

void Camera::setLrequested(bool newLrequested)
{
    m_lrequested = newLrequested;
}

DevicePropertyModel *Camera::model() const
{
    return m_settings;
}

bool Camera::isConnected() const
{
    return m_isConnected;
}

void Camera::setIsConnected(bool newIsConnected)
{
    if (m_isConnected == newIsConnected)
        return;
    m_isConnected = newIsConnected;
    emit isConnectedChanged();
}

bool Camera::isAcquiring() const
{
    return m_isAcquiring;
}

void Camera::setIsAcquiring(bool newIsAcquiring)
{
    if (m_isAcquiring == newIsAcquiring)
        return;
    m_isAcquiring = newIsAcquiring;
    emit isAcquiringChanged();
}

bool Camera::isSaving() const
{
    return m_isSaving;
}

void Camera::setIsSaving(bool newIsSaving)
{
    if (m_isSaving == newIsSaving)
        return;
    m_isSaving = newIsSaving;
    emit isSavingChanged();
}

int Camera::index() const
{
    return m_index;
}

void Camera::setIndex(int newIndex)
{
    if (m_index == newIndex)
        return;
    m_index = newIndex;
    emit indexChanged();
}

double Camera::hitthreshold() const
{
    return m_hitthreshold;
}

void Camera::setHitthreshold(double newHitthreshold)
{
    if (qFuzzyCompare(m_hitthreshold, newHitthreshold))
        return;
    m_hitthreshold = newHitthreshold;
    emit hitthresholdChanged();
}

int Camera::count() const
{
    return m_count;
}

void Camera::setCount(int newCount)
{
    if (m_count == newCount)
        return;
    m_count = newCount;
    emit countChanged();
}

int Camera::width() const
{
    return m_width;
}

void Camera::setWidth(int newWidth)
{
    if (m_width == newWidth)
        return;
    m_width = newWidth;
    emit widthChanged();
}

int Camera::height() const
{
    return m_height;
}

void Camera::setHeight(int newHeight)
{
    if (m_height == newHeight)
        return;
    m_height = newHeight;
    emit heightChanged();
}

bool Camera::showSum() const
{
    return m_showSum;
}

void Camera::setShowSum(bool newShowSum)
{
    if (m_showSum == newShowSum)
        return;
    m_showSum = newShowSum;
    emit showSumChanged();
}

int Camera::liverefreshrate() const
{
    return m_liverefreshrate;
}

void Camera::setLiverefreshrate(int newLiverefreshrate)
{
    if (m_liverefreshrate == newLiverefreshrate)
        return;
    m_liverefreshrate = newLiverefreshrate;
    emit liverefreshrateChanged();
}

int Camera::sumrefreshrate() const
{
    return m_sumrefreshrate;
}

void Camera::setSumrefreshrate(int newSumrefreshrate)
{
    if (m_sumrefreshrate == newSumrefreshrate)
        return;
    m_sumrefreshrate = newSumrefreshrate;
    emit sumrefreshrateChanged();
}
