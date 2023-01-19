#ifndef ACQUIREDATACARD_H
#define ACQUIREDATACARD_H

#include "global.h"
#include "qsemaphore.h"
#include <IviVisaType.h>
#include <QThread>
#include <QSemaphore>


class AcquireDataCard : public QThread
{
    Q_OBJECT
    void run() override;

signals:
    void runFinished();
    void errorOccured(int status);

public slots:
    void updateRecordSize(long recordsize);

public:
    AcquireDataCard(ViSession session,
                    int count,
                    QSemaphore *used,
                    QSemaphore *free,
                    LiveData *dataBuffer);
    ~AcquireDataCard();
    void stop();

private:
    void _run();

private:
    ViSession m_session;
    int m_count;
    bool m_flag;
    QSemaphore *m_used;
    QSemaphore *m_free;
    LiveData *m_dataBuffer;
    long m_recordsize = 0;
};

#endif // ACQUIREDATACARD_H
