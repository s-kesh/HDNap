#include "livedataconsumer.h"
#include <cstdint>
#include <iostream>
#include <QFile>
#include <QDataStream>
#include <string>

void LiveDataConsumer::run()
{
    m_flag = true;
    _run();
    stop();
}

LiveDataConsumer::LiveDataConsumer(int count,
                                   bool isSaving,
                                   QSemaphore *used,
                                   QSemaphore *free,
                                   LiveData *dataBuffer,
                                   std::string filepath)
    : m_count(count),
      m_saveflag(isSaving),
      m_flag(false),
      m_used(used),
      m_free(free),
      m_dataBuffer(dataBuffer),
      m_filepath(filepath)
{

}

LiveDataConsumer::~LiveDataConsumer()
{
    m_used = nullptr;
    m_free = nullptr;
    m_dataBuffer = nullptr;
}

void LiveDataConsumer::stop()
{
    m_flag = false;
}

void LiveDataConsumer::init(uint64_t size,
                            uint64_t rollingsize,
                            QList<QPointF> *live,
                            QList<QPointF> *rollavg,
                            QList<QPointF> *avg)
{
    m_size = size;
    m_rollingsize = rollingsize;
    m_realSum.resize(m_size);
    m_movingSum.resize(m_size);

    m_live = live;
    m_avg = avg;
    m_rollavg = rollavg;
}

//void LiveDataConsumer::init(std::uint64_t size,
//                            std::uint64_t rollingsize,
//                            std::int16_t *rollavgbuff,
//                            std::int16_t *avgbuff)
//{
//    m_size = size;
//    m_rollingsize = rollingsize;
//    m_realSum.resize(m_size);
//    m_movingSum.resize(m_size);
//    m_rollavg = rollavgbuff;
//    m_avg = avgbuff;
//}

void LiveDataConsumer::_run()
{
    int index = 0;
    LiveData *data = nullptr;
//    std::vector<double> xpoints;
//    xpoints.resize(m_size);

    std::string filename = m_filepath + "tof.bin";

    std::cout << "File Name " << filename << std::endl;
    QFile file(filename.c_str());
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    while (index < m_count && m_flag) {
        m_used->acquire();
        data = &m_dataBuffer[index % BUFFSIZE];
        std:int64_t firsttimepoint = data->firsttimepoint;

        if (m_size != data->size) {
            m_size = data->size;
            m_realSum.resize(m_size);
            m_movingSum.resize(m_size);
            m_live->resize(m_size);
            m_avg->resize(m_size);
            m_rollavg->resize(m_size);

            for (int i = 0; i < m_size; ++i) {
                m_live->operator[](i).setX(1000000 * (data->timeoffset + i * data->timefactor));
            }
            for (int i = 0; i < m_size; ++i) {
                m_avg->operator[](i).setX(1000000 * (data->timeoffset + i * data->timefactor));
            }
            for (int i = 0; i < m_size; ++i) {
                m_rollavg->operator[](i).setX(1000000 * (data->timeoffset + i * data->timefactor));
            }
        }

        double x = 0, y = 0;
        for (int i = 0; i < data->size; ++i) {
            y = *(data->data + data->firsttimepoint + i);
            y = data->voltoffset + y * data->voltfactor;
            y += index;
            m_live->operator[](i).setY(y);
        }


        for (size_t i = 0; i < m_size; ++i) {
            m_realSum[i] += *(data->data + i + firsttimepoint);
        }

        for (size_t i = 0; i < m_size; ++i) {
            y = data->voltoffset + (m_realSum[i] / (index + 1)) * data->voltfactor;
            y += index * 0.75;
            m_avg->operator[](i).setY(y);
        }

        for (size_t i = 0; i < m_size; ++i) {
            m_movingSum[i] += *(data->data + i + firsttimepoint);
        }

        if ( (index + 1) % m_rollingsize == 0) {
            for (size_t i = 0; i < m_size; ++i) {
                y = data->voltoffset + (m_movingSum[i] / (index + 1)) * data->voltfactor;
                m_rollavg->operator[](i).setY(y);
                m_movingSum[i] = 0;
            }
        }

        if (m_saveflag) {
            /* Save Image To Disk
            * Layout of Saved Binary File
            */

            out.writeRawData((char*)data, sizeof(LiveData) - 8);
            out.writeRawData((char*)data->data, firsttimepoint + m_size);
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

