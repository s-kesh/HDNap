#include "acquiredatacard.h"
#include "qelapsedtimer.h"
#include <AqMD3.h>

#include <QElapsedTimer>
#include <iostream>

void AcquireDataCard::run()
{
    m_flag = true;
    _run();
    stop();
}

void AcquireDataCard::updateRecordSize(long recordsize)
{
    m_recordsize = recordsize;
}

AcquireDataCard::AcquireDataCard(ViSession session,
                                 int count,
                                 QSemaphore *used,
                                 QSemaphore *free,
                                 LiveData *dataBuffer)
    : m_session(session),
      m_count(count),
      m_flag(false),
      m_used(used),
      m_free(free),
      m_dataBuffer(dataBuffer)
{

}

AcquireDataCard::~AcquireDataCard()
{
    m_used = nullptr;
    m_free = nullptr;
    m_dataBuffer = nullptr;
}

void AcquireDataCard::stop()
{
    m_flag = false;
}

void AcquireDataCard::_run()
{
    int index = 0;
    LiveData *data = nullptr;

    while (index < m_count && m_flag) {
        m_free->acquire();
//        std::cout << "Data Producer " << index << std::endl;


        QElapsedTimer timer;
        timer.start();
        while (timer.elapsed() <  1000) {

        }

        // Low level Acquisition
        AqMD3_InitiateAcquisition( m_session );
        AqMD3_SendSoftwareTrigger(m_session);
        AqMD3_WaitForAcquisitionComplete( m_session, 10000 );

        // Fetch Waveform
        data = &m_dataBuffer[index % BUFFSIZE];
        ViStatus status = AqMD3_FetchWaveformInt16( m_session,
                                                    "Channel1",
                                                    m_recordsize + 100,
                                                    data->data,
                                                    &(data->size),
                                                    &(data->firsttimepoint),
                                                    &(data->timeoffset),
                                                    &(data->timestampseconds),
                                                    &(data->timestampsfractions),
                                                    &(data->timefactor),
                                                    &(data->voltfactor),
                                                    &(data->voltoffset));

        if (status != 0) {
            // Emit Error
            emit errorOccured(status);
        }

        ++index;
        m_used->release();

    }
    emit runFinished();
    m_used->release(BUFFSIZE);
}
