#include "liveimageconsumer.h"
#include "global.h"
#include <iostream>
#include <QFile>
#include <QDataStream>
#include <string>

#include <Eigen/Dense>

void LiveImageConsumer::run()
{
    m_flag = true;
    _run();
    stop();
}

LiveImageConsumer::LiveImageConsumer(int count,
                                     bool saveflag,
                                     QSemaphore *used,
                                     QSemaphore *free,
                                     LiveImage *imageBuffer,
                                     std::string filepath)
    : m_count(count),
      m_saveflag(saveflag),
      m_flag(false),
      m_used(used),
      m_free(free),
      m_imageBuffer(imageBuffer),
      m_filepath(filepath)
{

}

LiveImageConsumer::~LiveImageConsumer()
{
    m_used = nullptr;
    m_free = nullptr;
    m_imageBuffer = nullptr;
}

void LiveImageConsumer::stop()
{
    m_flag = false;
}

void LiveImageConsumer::init(double thres,
                             std::uint64_t size,
                             uint8_t *sumbuff,
                             uint8_t *avgbuff)
{
    m_thres = thres;
    m_sum = sumbuff;
    m_avg = avgbuff;
    m_size = size;
    m_sumReal.resize(m_size);
    m_sumReal.setZero();
}

void LiveImageConsumer::_run()
{
    int index = 0;
    LiveImage *image = nullptr;

    std::string filename = m_filepath + "_image.bin";
    QFile file(filename.c_str());
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    MapTypeU8 avg(m_avg, m_size);

    while (index < m_count && m_flag) {
        m_used->acquire();
        image = &m_imageBuffer[index % BUFFSIZE];

        if (m_size != image->size) {
            m_size = image->size;
            m_sumReal.resize(m_size);
        }

        MapTypeConstU8 live(image->data, image->size);
        MatrixTypef livef = live.cast<float>();

        if (livef.sum() / m_size > m_thres) {
            // It is a hit
            image->hit = true;
        }

        m_sumReal += livef;

        if (m_requested) {
            // Need to clamp sum at 255 otherwise it will overflow
            // It is very slow operation
            // need to find some fast way (maybe GPU?)
            for (size_t i = 0; i < m_size; ++i) {
                if (m_sumReal[i] > 255)
                    *(m_sum + i) = 255;
                else
                    *(m_sum + i) = m_sumReal[i];
            }

            avg = (m_sumReal / (index + 1)).cast<std::uint8_t>();
            m_requested = false;
        }


        if (m_saveflag) {
            /* Save Image To Disk
            * Layout of Saved Binary File
            * <1 byte> <7 bytes> <8 bytes> <8 bytes> <8 bytes> <8 bytes> <m_size bytes>
            * <Hit> <Padding> <Index> <ID> <TimeStamp> <Size> <Image Data>
            * While plotting, please cast Image Data to a uint8_t type
            * Cast 8 bytes to uint64_t type
            */

            out.writeRawData((char*)image, sizeof(LiveImage) - 8);
            out.writeRawData((char*)image->data, m_size);
        }

        emit resultReady(index);

        ++index;
        m_free->release();
    }

    file.close();

    if (!m_saveflag) {
        file.remove();
    }

    emit runFinished();
    m_free->release(BUFFSIZE);
}

bool LiveImageConsumer::requested() const
{
    return m_requested;
}

void LiveImageConsumer::setRequested(bool newRequested)
{
    m_requested = newRequested;
}
