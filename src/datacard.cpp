#include "datacard.h"
#include "acquiredatacard.h"
#include "global.h"
#include "livedataconsumer.h"
#include <AqMD3.h>
#include <IviVisaType.h>
#include <iostream>
#include <QAbstractAxis>
#include <QAbstractSeries>


QT_USE_NAMESPACE

Q_DECLARE_METATYPE(QAbstractSeries *)
Q_DECLARE_METATYPE(QAbstractAxis *)

//enum TriggerSource {
//    Internal1,
//    Internal2,
//    External1,
//    External2,
//    Software,
//    SelfTrigger
//};

int intfromtriggersourcestring(std::string source) {
    int selection = 0;
    if (source == "Internal1")
        selection = 0;
    else if (source == "Internal2")
        selection = 1;
    else if (source == "External1")
        selection = 2;
    else if (source == "Imediate")
        selection = 3;
    else if (source == "Software")
        selection = 4;
    else if (source == "SelfTrigger")
        selection = 5;
    return selection;
}

std::string stringfromtriggersourceint(int selection) {
    std::string source = "";
    switch (selection) {
        case 0:
            source = "Internal1";
            break;
        case 1:
            source = "Internal2";
            break;
        case 2:
            source = "External1";
            break;
        case 3:
            source = "Imediate";
            break;
        case 4:
            source = "Software";
            break;
        case 5:
            source = "SelfTrigger";
            break;
        default:
            source = "External1";
    }
    return source;
}

#define checkApiCall( f ) do { ViStatus s = f; testApiCall( s, #f ); } while( false )
// Utility function to check status error during driver API call.
void testApiCall( ViStatus status, char const * functionName )
{
    ViInt32 ErrorCode;
    ViChar ErrorMessage[512];

    if( status>0 ) // Warning occurred.
    {
        AqMD3_GetError( VI_NULL, &ErrorCode, sizeof( ErrorMessage ), ErrorMessage );
        std::cerr << "** Warning during " << functionName << ": 0x" << std::hex << ErrorCode << ", " << ErrorMessage << '\n';
    }
    else if( status<0 ) // Error occurred.
    {
        AqMD3_GetError( VI_NULL, &ErrorCode, sizeof( ErrorMessage ), ErrorMessage );
        std::cerr << "** ERROR during " << functionName << ": 0x" << std::hex << ErrorCode << ", " << ErrorMessage << '\n';
        throw std::runtime_error( ErrorMessage );
    }
}

DataCard::DataCard(QObject *parent)
    : QObject{parent},
      m_settings(new DevicePropertyModel),
      m_isConnected(false),
      m_isAcquiring(false),
      m_isSaving(false),
      m_index(0),
      m_count(0)
{

    qRegisterMetaType<QAbstractSeries*>();
    qRegisterMetaType<QAbstractAxis*>();

    /* Properties of Acqiris DataCard
     * Serial No
     *
     * Channel
     * Range
     * Offset
     * RecordSize
     *
     * Trigger Source
     * Trigger Delay
     * Trigger Level
     */

    m_settings->addItem("Serial No", "Not Connected", 0, 0, 0, 0);
    m_propmap.insert("Serial No", m_settings->index(0));

    m_settings->addItem("Channel No", 0, 0, 1, 1, 0);
    m_propmap.insert("Channel No", m_settings->index(1));

    m_settings->addItem("Range", 1, 0.05, 5.0, 0.01, 3);
    m_propmap.insert("Range", m_settings->index(2));
    m_settings->addItem("Offset", 0, 0, 5.0, 0.01, 3);
    m_propmap.insert("Offset", m_settings->index(3));
    m_settings->addItem("Record Size", 10000, 1024, 1000000, 4, 0);
    m_propmap.insert("Record Size", m_settings->index(4));

    m_settings->addItem("Trigger Source", 0, 0, 5, 1, 0);
    m_propmap.insert("Trigger Source", m_settings->index(5));
    m_settings->addItem("Trigger Delay", 0, -100, 100, 0.01, 2);
    m_propmap.insert("Trigger Delay", m_settings->index(6));
    m_settings->addItem("Trigger Level", 1, 0, 3, 0.01, 3);
    m_propmap.insert("Trigger Level", m_settings->index(7));
}

DataCard::~DataCard()
{

}

DevicePropertyModel *DataCard::model() const
{
    return m_settings;
}

bool DataCard::showSum() const
{
    return m_showSum;
}

void DataCard::setShowSum(bool newShowSum)
{
    if (m_showSum == newShowSum)
        return;
    m_showSum = newShowSum;
    emit showSumChanged();
}

bool DataCard::isConnected() const
{
    return m_isConnected;
}

void DataCard::setIsConnected(bool newIsConnected)
{
    if (m_isConnected == newIsConnected)
        return;
    m_isConnected = newIsConnected;
    emit isConnectedChanged();
}

bool DataCard::isAcquiring() const
{
    return m_isAcquiring;
}

void DataCard::setIsAcquiring(bool newIsAcquiring)
{
    if (m_isAcquiring == newIsAcquiring)
        return;
    m_isAcquiring = newIsAcquiring;
    emit isAcquiringChanged();
}

bool DataCard::isSaving() const
{
    return m_isSaving;
}

void DataCard::setIsSaving(bool newIsSaving)
{
    if (m_isSaving == newIsSaving)
        return;
    m_isSaving = newIsSaving;
    emit isSavingChanged();
}

int DataCard::index() const
{
    return m_index;
}

void DataCard::setIndex(int newIndex)
{
    if (m_index == newIndex)
        return;
    m_index = newIndex;
    emit indexChanged();
}

int DataCard::count() const
{
    return m_count;
}

void DataCard::setCount(int newCount)
{
    if (m_count == newCount)
        return;
    m_count = newCount;
    emit countChanged();
}

void DataCard::startsession()
{
    if (m_isConnected) {
        // Already Connected
        return;
    }

    ViChar resource[] = "PXI40::0::0::INSTR";
//    ViChar options[]  = "Cache=true";
    ViChar options[] = "Simulate=true, DriverSetup= Model=U5303A";
    ViBoolean idQuery = VI_FALSE;
    ViBoolean reset = VI_FALSE;

    ViSession session;
    ViStatus status = AqMD3_InitWithOptions(resource, idQuery, reset, options, &session);
    if (status != 0) {
        // Unable to connect
        return;
    }

    // save session id
    m_session = session;

    // Get Serial No
    char serialno[128];
    AqMD3_GetAttributeViString( m_session, "", AQMD3_ATTR_INSTRUMENT_INFO_SERIAL_NUMBER_STRING
                                , sizeof(serialno ), serialno ) ;
    m_settings->setData(m_propmap["Serial No"],
                        QVariant::fromValue(QString(serialno)),
                        m_settings->ValueRole);
    // Enable both channels
    AqMD3_SetAttributeViBoolean(m_session, "Channel1", AQMD3_ATTR_CHANNEL_ENABLED, 1);
    AqMD3_SetAttributeViBoolean(m_session, "Channel2", AQMD3_ATTR_CHANNEL_ENABLED, 1);

    // Calibrate
    ViBoolean flag;
    AqMD3_GetAttributeViBoolean(m_session,
                                "",
                                AQMD3_ATTR_CALIBRATION_IS_REQUIRED,
                                &flag);
    if (flag) {
        qDebug() << "Calibrating";
        AqMD3_SelfCalibrate(m_session);
    }

    // Initialise Time
    AqMD3_SetTime(m_session, 0, 0) ;

    setIsConnected(true);
}

void DataCard::stopsession()
{
    if (!m_isConnected) {
        // Not Connected
        return;
    }

    if (m_isAcquiring) {
        // Already Acquiring data
        // Stop that
    }

    AqMD3_close(m_session);
    setIsConnected(false);

}

void DataCard::getProperties()
{
    if (!m_isConnected) {
        // Not Connected
        return;
    }

    /* Properties of Acqiris DataCard
     * Serial No
     *
     * Channel
     * Range
     * Offset
     * RecordSize
     *
     * Trigger Source
     * Trigger Delay
     * Trigger Level
     */

    char serialno[128];
    AqMD3_GetAttributeViString( m_session, "",
                                AQMD3_ATTR_INSTRUMENT_INFO_SERIAL_NUMBER_STRING,
                                sizeof(serialno ), serialno ) ;
    m_settings->setData(m_propmap["Serial No"],
                        QVariant::fromValue(QString(serialno)),
                        m_settings->ValueRole);


    std::double_t range;
    std::double_t offset;
    std::int64_t recordsize;

    // Get Selected Channel
    std::string channelname = (m_settings->data(m_propmap["Channel No"],
                           m_settings->ValueRole).toInt() == 0)? "Channel1" : "Channel2";

    std::cout << channelname << std::endl;


    AqMD3_GetAttributeViReal64(m_session, channelname.c_str(),
                               AQMD3_ATTR_VERTICAL_OFFSET, &offset);
    AqMD3_GetAttributeViReal64(m_session, channelname.c_str(),
                               AQMD3_ATTR_VERTICAL_RANGE, &range);

    AqMD3_GetAttributeViInt64(m_session, "",
                              AQMD3_ATTR_RECORD_SIZE,
                              &recordsize);



    m_settings->setData(m_propmap["Range"],
                        QVariant::fromValue(range),
                        m_settings->ValueRole);
    m_settings->setData(m_propmap["Offset"],
                        QVariant::fromValue(offset),
                        m_settings->ValueRole);
    m_settings->setData(m_propmap["Record Size"],
                        QVariant::fromValue(recordsize),
                        m_settings->ValueRole);

    char source[125];
    std::double_t tdelay;
    std::double_t tlevel;

    AqMD3_GetAttributeViString(m_session, "",
                               AQMD3_ATTR_ACTIVE_TRIGGER_SOURCE,
                               sizeof(source), source);
    AqMD3_GetAttributeViReal64(m_session, "",
                               AQMD3_ATTR_TRIGGER_DELAY, &tdelay);
    AqMD3_GetAttributeViReal64(m_session, source,
                               AQMD3_ATTR_TRIGGER_LEVEL, &tlevel);


    // Should be a better way
    QString tempsource = QString(QLatin1String(source));
    int val = intfromtriggersourcestring(tempsource.toStdString());
    m_settings->setData(m_propmap["Trigger Source"],
                            QVariant::fromValue(val),
                            m_settings->ValueRole);
    m_settings->setData(m_propmap["Trigger Delay"],
                        QVariant::fromValue(tdelay),
                        m_settings->ValueRole);
    m_settings->setData(m_propmap["Trigger Level"],
                        QVariant::fromValue(tlevel),
            m_settings->ValueRole);
}

void DataCard::setProperties()
{
    if (!isConnected()) {
        // It is not connected
        // Issue warning and return
//        emit warning("Not Connected\n");
        return;
    }

    if (isAcquiring()) {
//        emit warning("It is Acquiring, stop the run and restart\n");
        return;
    }

    int channelno = m_settings->data(m_propmap["Channel No"],
            m_settings->ValueRole).toInt();
    std::double_t range = m_settings->data(m_propmap["Range"],
                                           m_settings->ValueRole).toDouble();
    std::double_t offset = m_settings->data(m_propmap["Offset"],
                                            m_settings->ValueRole).toDouble();
    std::int64_t recordsize = m_settings->data(m_propmap["Record Size"],
                                               m_settings->ValueRole).toInt();

    std::string channelname = (channelno == 0)? "Channel1" : "Channel2";

    std::cout << channelname << std::endl;

    AqMD3_SetAttributeViReal64(m_session,
                               channelname.c_str(),
                               AQMD3_ATTR_VERTICAL_RANGE,
                               range);
    AqMD3_SetAttributeViReal64(m_session,
                               channelname.c_str(),
                               AQMD3_ATTR_VERTICAL_OFFSET,
                               offset);
    AqMD3_SetAttributeViInt64(m_session, "",
                              AQMD3_ATTR_RECORD_SIZE,
                              recordsize);

    std::string tsource = stringfromtriggersourceint(
                m_settings->data(m_propmap["Trigger Source"],
                            m_settings->ValueRole).toInt());

    std::cout << tsource << std::endl;


    std::double_t tdelay = m_settings->data(m_propmap["Trigger Delay"],
                        m_settings->ValueRole).toDouble();
    std::double_t tlevel = m_settings->data(m_propmap["Trigger Level"],
            m_settings->ValueRole).toDouble();

    AqMD3_SetAttributeViString(m_session, "",
                               AQMD3_ATTR_ACTIVE_TRIGGER_SOURCE,
                               tsource.c_str());
    AqMD3_SetAttributeViReal64(m_session, "",
                               AQMD3_ATTR_TRIGGER_DELAY, tdelay);
    AqMD3_SetAttributeViReal64(m_session, tsource.c_str(),
                               AQMD3_ATTR_TRIGGER_LEVEL, tlevel);

}

void DataCard::acquireData(const int count, const QString filename)
{

    if (!isConnected()) {
        // It is not connected
        // Issue warning and return
//        emit warning("Not Connected\n");
        return;
    }

    if (isAcquiring()) {
//        emit warning("It is Acquiring, stop the run and restart\n");
        return;
    }

    // set Written Values
    setProperties();

    std::int64_t recordsize = m_settings->data(m_propmap["Record Size"],
                                               m_settings->ValueRole).toInt();
    recordsize = 96;

    m_size = recordsize;

    dataBuffer = new LiveData [BUFFSIZE];
    usedSemp = new QSemaphore;
    freeSemp = new QSemaphore(BUFFSIZE);
    for (int i = 0; i < BUFFSIZE; ++i)
        (dataBuffer + i)->data = new std::int16_t [recordsize + 100];

    m_xlive = new float [recordsize];
    m_ylive = new float [recordsize];
    m_yavg = new float [recordsize];
    m_yrollavg = new float [recordsize];

    for (int i = 0; i < recordsize; ++i) {
        m_xlive[i] = i;
        m_ylive[i] = 0;
        m_yavg[i] = 0;
        m_yrollavg[i] = 0;
    }


    setCount(count);
    setIndex(-1);

//    rollavgData.clear();
//    avgData.clear();

    m_liveSeries.clear();
    m_avgSeries.clear();
    m_rollavgSeries.clear();
    m_liveSeries.resize(recordsize);
    m_avgSeries.resize(recordsize);
    m_rollavgSeries.resize(recordsize);

    // Setup Producer thread
    m_producer = new AcquireDataCard(m_session,
                                   m_count,
                                   usedSemp,
                                   freeSemp,
                                   dataBuffer);

    // Setup Consumer thread
    m_consumer = new LiveDataConsumer(m_count,
                                       m_isSaving,
                                       usedSemp,
                                       freeSemp,
                                       dataBuffer,
                                       filename.toStdString());

    connect(m_consumer,
            &LiveDataConsumer::resultReady,
            this,
            &DataCard::handleSignal);

//    rollavgData.resize(recordsize);
//    avgData.resize(recordsize);

    m_liveSeries.resize(recordsize);
    m_avgSeries.resize(recordsize);
    m_rollavgSeries.resize(recordsize);

    // Is Calibration Required?
    ViBoolean isCalibrationRequired;
    AqMD3_GetAttributeViBoolean(m_session,
                                "",
                                AQMD3_ATTR_CALIBRATION_IS_REQUIRED,
                                &isCalibrationRequired);

    if (isCalibrationRequired) {
        AqMD3_SelfCalibrate(m_session);
    }

    // Start Consumer

//    m_consumer->init(recordsize,
//                     100,
//                     m_liveSeries.data(),
//                     m_rollavgSeries.data(),
//                     m_avgSeries.data());

    m_consumer->init(recordsize,
                     100,
                     m_xlive,
                     m_ylive,
                     m_yrollavg,
                     m_yavg);

    m_consumer->start();

    // start thread
    m_producer->start();

    setIsAcquiring(true);
}

void DataCard::stopacquiringData()
{
    if (!m_isConnected) {
        // Connect hi nahi hain
        return;
    }

    if (!isAcquiring()) {
        // Already not Acquiring
//        emit warning("Nahi ho raha hain Acquire");
        return;
    }

    if (m_producer->isRunning()) {
        m_producer->stop();
        m_producer->wait();
    }

    if (m_consumer->isRunning()) {
        m_consumer->stop();
        m_consumer->wait();
    }

    delete m_consumer;

    delete m_producer;
    delete usedSemp;
    delete freeSemp;

    for (int i = 0; i < BUFFSIZE; ++i)
        delete [] (dataBuffer + i)->data;

    delete [] dataBuffer;

    delete [] m_xlive;
    delete [] m_ylive;
    delete [] m_yavg;
    delete [] m_yrollavg;


    setIsAcquiring(false);
}

void DataCard::handleSignal(uint64_t index)
{
    if (!m_isAcquiring) {
        // Bhaiya Acquire hi nahi kar rahe hain to dekhayege kiya
        return;
    }
    setIndex(index);

//    emit resultReady();

    if (m_index == m_count - 1)
        stopacquiringData();
}

void DataCard::updateSeries(QAbstractSeries *series)
{
    if (!m_isAcquiring) {
        // Bhaiya Acquire hi nahi kar rahe hain to dekhayege kiya
        return;
    }

    if (m_index > 1) {
        for (int i = 0; i < m_size; ++i) {

            m_liveSeries[i].setX(m_xlive[i]);
            m_liveSeries[i].setY(m_ylive[i]);
        }


        QXYSeries *xySeries = static_cast<QXYSeries *>(series);
        xySeries->replace(m_liveSeries);

    }
}

void DataCard::updateAvgSeries(bool flag,
                               QAbstractSeries *series)
{
    if (!m_isAcquiring) {
        // Bhaiya Acquire hi nahi kar rahe hain to dekhayege kiya
        return;
    }

    if (m_index > 1) {
        QXYSeries *xySeries = static_cast<QXYSeries *>(series);
        if (flag) {
            for (int i = 0; i < m_size; ++i) {
                m_avgSeries[i].setX(m_xlive[i]);
                m_avgSeries[i].setY(m_yavg[i]);
            }
            xySeries->replace(m_avgSeries);
        }
        else {
            for (int i = 0; i < m_size; ++i) {
                m_rollavgSeries[i].setX(m_xlive[i]);
                m_rollavgSeries[i].setY(m_yrollavg[i]);
            }
            xySeries->replace(m_rollavgSeries);
        }
    }
}

void DataCard::Aq_ErrorHandler(long error)
{
    ViInt32 ErrorCode;
    ViChar ErrorMessage[512];
    AqMD3_GetError(VI_NULL, &ErrorCode, sizeof(ErrorMessage), ErrorMessage);

    /*
    if (error > 0)  // Just a Warning
        emit errormessage(ErrorMessage);
    else if (error < 0) // An Error
        emit errormessage(ErrorMessage);
        */
}
