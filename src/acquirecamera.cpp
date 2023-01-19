#include "acquirecamera.h"
#include "global.h"
#include "pylon/ETimeoutHandling.h"
#include <cstdint>
#include <cstring>
#include <QDebug>
#include <QElapsedTimer>

void AcquireCamera::run()
{
    m_flag = true;
    _run();
    stop();
}

AcquireCamera::AcquireCamera(Pylon::CInstantCamera *camera,
                             int count,
                             QSemaphore *used,
                             QSemaphore *free,
                             LiveImage *imageBuffer)
    : m_camera(camera),
      m_count(count),
      m_flag(false),
      m_used(used),
      m_free(free),
      m_imageBuffer(imageBuffer)
{

}

AcquireCamera::~AcquireCamera()
{
    m_camera = nullptr;
    m_used = nullptr;
    m_free = nullptr;
    m_imageBuffer = nullptr;
}

void AcquireCamera::stop()
{
    m_flag = false;
}

void AcquireCamera::_run()
{
    int index = 0;
    LiveImage *image = nullptr;

    while (index < m_count && m_flag) {
        m_free->acquire();
        if (m_camera->IsGrabbing()) {
            try {
                m_camera->RetrieveResult(10000,
                             ptrGrab,
                             Pylon::TimeoutHandling_Return);
                if (ptrGrab->GrabSucceeded()) {
                    image = &m_imageBuffer[index % BUFFSIZE];

                    image->index = index;
                    image->id = ptrGrab->GetID();
                    image->timestamp = ptrGrab->GetTimeStamp();
                    image->size = ptrGrab->GetBufferSize();

                    // I really do not want to copy complete buffer
                    // Ideally, I just need to pass pointer
                    // and copy buffer only on write
                    // But this is need as taking sum and average of images takes time
                    // So, we need a copy.

                    std::memcpy(image->data,
                                (std::uint8_t *)ptrGrab->GetBuffer(),
                                image->size);
                    // As we have already copied the buffer, we can release it
                    ptrGrab.Release();
                    // image->data = (std::uint8_t *)ptrGrab->GetBuffer();
                }
            }
            catch (const GenICam_3_1_Basler_pylon::GenericException &e) {
                std::cout << "Capture Problem " << e.what() << std::endl;
            }
        }

        ++index;
        m_used->release();
    }
    emit runFinished();
    m_used->release(BUFFSIZE);
}
