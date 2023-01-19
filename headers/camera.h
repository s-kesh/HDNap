#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <qqml.h>
#include <QString>

#include "devicepropertymodel.h"
#include "global.h"
#include "acquirecamera.h"
#include "liveimageconsumer.h"

#include <pylon/PylonIncludes.h>

class Camera : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DevicePropertyModel* settings READ model CONSTANT)

    // Width and Height of Image
    // Need in QML to show currect aspect ratio of image
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)

    // Flag to emit sum or average
    Q_PROPERTY(bool showSum READ showSum WRITE setShowSum NOTIFY showSumChanged)

    // Status Flags
    Q_PROPERTY(bool isConnected READ isConnected WRITE setIsConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(bool isAcquiring READ isAcquiring WRITE setIsAcquiring NOTIFY isAcquiringChanged)
    Q_PROPERTY(bool isSaving READ isSaving WRITE setIsSaving NOTIFY isSavingChanged)

    // Picture Index
    Q_PROPERTY(int index READ index WRITE setIndex NOTIFY indexChanged)
    Q_PROPERTY(int count READ count WRITE setCount NOTIFY countChanged)

    // Is it a hit?
    Q_PROPERTY(double hitthreshold READ hitthreshold WRITE setHitthreshold NOTIFY hitthresholdChanged)

    QML_SINGLETON

public:
    explicit Camera(QObject *parent = nullptr);
    ~Camera();

    // Model for setting
    DevicePropertyModel *model() const;

    bool isConnected() const;
    void setIsConnected(bool newIsConnected);
    bool isAcquiring() const;
    void setIsAcquiring(bool newIsAcquiring);
    bool isSaving() const;
    void setIsSaving(bool newIsSaving);
    int index() const;
    void setIndex(int newIndex);
    double hitthreshold() const;
    void setHitthreshold(double newHitthreshold);
    int count() const;
    void setCount(int newCount);
    int width() const;
    void setWidth(int newWidth);
    int height() const;
    void setHeight(int newHeight);
    bool showSum() const;
    void setShowSum(bool newShowSum);

public slots:
    void getProperties();
    void setProperties();
    void startsession();
    void stopsession();

    // Slot to Acquire and Handle images
    void acquireImages(const int count,
                       const QString filename);
    void stopacquiringImages();
    void handleSignal(std::uint64_t index);

    // To emit either averaged or summed signal
    // It is needed because
    // I am not saving average or sum image
    // In graphics part
    void forceEmit();

signals:
    void isConnectedChanged();
    void isAcquiringChanged();
    void isSavingChanged();
    void indexChanged();
    void hitthresholdChanged();
    void countChanged();
    void widthChanged();
    void heightChanged();
    void showSumChanged();

    void itisaHit(std::uint64_t index,
                     std::uint64_t width,
                     std::uint64_t height,
                     std::uint8_t *data);



    // send warning
    void warning(const QString str);
    // send error
    void error(const QString str);

    // Emit Live Frame Pointer
    void resultReady(std::uint64_t index,
                     std::uint64_t width,
                     std::uint64_t height,
                     std::uint8_t *data);

    // Emit either sum or average frame pointer
    void avgReady(std::uint64_t index,
                  std::uint64_t width,
                  std::uint64_t height,
                  std::uint8_t *data);


private:
    DevicePropertyModel *m_settings = nullptr;
    bool m_isConnected = false;
    bool m_isAcquiring = false;
    bool m_isSaving = false;
    int m_index = -1;
    double m_hitthreshold = 0;

    // Camera
    Pylon::CInstantCamera *m_camera = nullptr;
    // Map for properties
    QMap<QString, QModelIndex> m_propmap;

    // Buffer of grabbed Images
    LiveImage* imageBuffer = nullptr;

    // Buffer to store sum and avg image
    std::vector<std::uint8_t> sumImage;
    std::vector<std::uint8_t> avgImage;

    // Hits
    LiveImage *hitBuffer = nullptr;

    // Threads

    // Producer
    AcquireCamera *m_producer = nullptr;
    // Consumer
    LiveImageConsumer *m_consumer = nullptr;

    // Saver
    int m_count = 0;

    QSemaphore *usedSemp = nullptr;
    QSemaphore *freeSemp = nullptr;
    int m_width;
    int m_height;
    bool m_showSum = true;
};

#endif // CAMERA_H
