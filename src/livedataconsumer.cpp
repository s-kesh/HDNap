#include "livedataconsumer.h"
#include "Eigen/src/Core/Matrix.h"
#include "global.h"
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
                            float *x,
                            float *y,
                            float *rollavg,
                            float *avg)
{
    m_size = size;
    m_rollingsize = rollingsize;

    m_realSum.resize(m_size);
    m_movingSum.resize(m_size);
    ivec.resize(m_size);
    m_realSum.setZero();
    m_movingSum.setZero();

    m_x = x;
    m_y = y;
    m_avg = avg;
    m_rollavg = rollavg;

    for (int i = 0; i < m_size; ++i) {
        ivec[i] = i;
    }
}

void LiveDataConsumer::_run()
{
    int index = 0;
    LiveData *data = nullptr;

    m_requested = true;

    std::string filename = m_filepath + "_tof.bin";
    QFile file(filename.c_str());
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    MapTypef x(m_x, m_size);
    MapTypef y(m_y, m_size);
    MapTypef avg(m_avg, m_size);
    MapTypef rollavg(m_rollavg, m_size);

    while (index < m_count && m_flag) {
        m_used->acquire();
        data = &m_dataBuffer[index % BUFFSIZE];
        std:int64_t firsttimepoint = data->firsttimepoint;

        if (m_size != data->size) {
            m_size = data->size;
            m_realSum.resize(m_size);
            m_movingSum.resize(m_size);
        }

        MapTypeConst16 llive(data->data + firsttimepoint, m_size);


        float timefac = data->timefactor;
        MatrixTypef timeoff(m_size);
        timeoff.setConstant(data->timeoffset);

        x = 1000000 * (timeoff + ivec * timefac);

        float voltfac = data->voltfactor;
        MatrixTypef voltoff(m_size);
        voltoff.setConstant(index); //data->voltoffset);

        MatrixTypef livef = llive.cast<float>();

        m_realSum += livef;
        m_movingSum += livef;

        y = livef * voltfac + voltoff;
        avg = (m_realSum / (index + 1))* voltfac + voltoff;

        if ( (index + 1) % m_rollingsize == 0) {
            rollavg = (m_movingSum / (index + 1)) * voltfac + voltoff;
            m_movingSum.setZero();
        }

        if (m_saveflag) {
            /* Save Image To Disk
            * Layout of Saved Binary File
            */

            out.writeRawData((char*)data, sizeof(LiveData) - 8);
            out.writeRawData((char*)(data->data + firsttimepoint), m_size);
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

