#ifndef DATACARD_H
#define DATACARD_H

#include <QObject>
#include <QSemaphore>
#include <qqml.h>
#include <QList>
#include <QPointF>
#include <QXYSeries>

#include "acquiredatacard.h"
#include "devicepropertymodel.h"
#include "global.h"
#include "livedataconsumer.h"
#include "qabstractseries.h"


class DataCard : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DevicePropertyModel* settings READ model CONSTANT)

    // Flag to emit sum or average
    Q_PROPERTY(bool showSum READ showSum WRITE setShowSum NOTIFY showSumChanged)

    // Status Flags
    Q_PROPERTY(bool isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(bool isAcquiring READ isAcquiring WRITE setIsAcquiring NOTIFY isAcquiringChanged)
    Q_PROPERTY(bool isSaving READ isSaving WRITE setIsSaving NOTIFY isSavingChanged)

    // Picture Index
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

    QML_SINGLETON

public:
    explicit DataCard(QObject *parent = nullptr);
    ~DataCard();

    DevicePropertyModel *model() const;

    bool showSum() const;
    void setShowSum(bool newShowSum);

    bool isConnected() const;
    void setIsConnected(bool newIsConnected);

    bool isAcquiring() const;
    void setIsAcquiring(bool newIsAcquiring);

    bool isSaving() const;
    void setIsSaving(bool newIsSaving);

    int index() const;
    void setIndex(int newIndex);

    int count() const;
    void setCount(int newCount);

public slots:
    // Connect
    void startsession();
    // Disconnect
    void stopsession();
    // Get Properties
    void getProperties();
    // Set Properties
    void setProperties();

    // Slots to Acquire and Handle data
    void acquireData(const int count,
                       const QString filename);
    void stopacquiringData();
    void handleSignal(std::uint64_t index);

    void updateSeries(QAbstractSeries *series);
    void updateAvgSeries(bool flag,
                         QAbstractSeries *series);


    // Handle Internal Error
    void Aq_ErrorHandler(long error);

signals:
    void showSumChanged();
    void isConnectedChanged();
    void isAcquiringChanged();
    void isSavingChanged();
    void indexChanged();
    void countChanged();
    void erroraccoured(long error);

    // Emit acquired index
    void resultReady();

private:
    DevicePropertyModel *m_settings = nullptr;
    bool m_showSum = true;
    bool m_isConnected = false;
    bool m_isAcquiring = false;
    bool m_isSaving = false;
    int m_index;
    int m_count;

    // DataCard
    unsigned int m_session;

    // Map for properties
    QMap<QString, QModelIndex> m_propmap;

    // Buffer to grab Data
    LiveData* dataBuffer = nullptr;

    // Threads

    // Producer thread
    AcquireDataCard *m_producer = nullptr;
    // Consumer thread
    LiveDataConsumer *m_consumer = nullptr;

    QSemaphore *usedSemp = nullptr;
    QSemaphore *freeSemp = nullptr;

    // Three XYSeries to plot
    QList<QPointF> m_liveSeries;
    QList<QPointF> m_avgSeries;
    QList<QPointF> m_rollavgSeries;

    float *m_xlive = nullptr;
    float *m_ylive = nullptr;
    float *m_yavg = nullptr;
    float *m_yrollavg = nullptr;

    std::size_t m_size;

};

#endif // DATACARD_H
