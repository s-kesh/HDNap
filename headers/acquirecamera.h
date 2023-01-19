#ifndef ACQUIRECAMERA_H
#define ACQUIRECAMERA_H

#include <QThread>
#include <QSemaphore>

#include <pylon/PylonIncludes.h>
#include "global.h"
#include <pylon/InstantCamera.h>

class AcquireCamera : public QThread
{
    Q_OBJECT

    void run() override;

signals:
    void runFinished();
//    void resultReady(LiveImage *imgBuff);

public:
    AcquireCamera(Pylon::CInstantCamera *camera,
                  int count,
                  QSemaphore *used,
                  QSemaphore *free,
                  LiveImage *imageBuffer);

    ~AcquireCamera();
    void stop();
    Pylon::CGrabResultPtr ptrGrab;

private:
    void _run();

private:
    Pylon::CInstantCamera *m_camera;
    int m_count;
    bool m_flag;
    QSemaphore *m_used;
    QSemaphore *m_free;
    LiveImage *m_imageBuffer;
};

#endif // ACQUIRECAMERA_H
