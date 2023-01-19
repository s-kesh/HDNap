#ifndef LIVEDATACONSUMER_H
#define LIVEDATACONSUMER_H

#include <QThread>
#include <QSemaphore>
#include "global.h"
#include "qlist.h"
#include "qpoint.h"

class LiveDataConsumer : public QThread
{
    Q_OBJECT
    void run() override;

public:
    LiveDataConsumer(int count,
                      bool isSaving,
                      QSemaphore *used,
                      QSemaphore *free,
                      LiveData *dataBuffer,
                      std::string filepath);
    ~LiveDataConsumer();
    void stop();

signals:
    void resultReady(std::uint64_t index);
    void runFinished();

public slots:
    void init (std::uint64_t size,
               std::uint64_t rollingsize,
               QList<QPointF> *live,
               QList<QPointF> *rollavg,
               QList<QPointF> *avg);

private:
    void _run();

private:
    int m_count;
    bool m_flag = false;
    QSemaphore *m_used;
    QSemaphore *m_free;
    LiveData *m_dataBuffer;

    std::vector<std::int32_t> m_realSum;
    std::vector<std::int32_t> m_movingSum;
    std::uint64_t m_size = 0;
    std::uint64_t m_rollingsize = 0;

//    std::int16_t *m_rollavg = nullptr;
//    std::int16_t *m_avg = nullptr;

    QList<QPointF> *m_live = nullptr;
    QList<QPointF> *m_avg = nullptr;
    QList<QPointF> *m_rollavg = nullptr;

    std::string m_filepath;
    bool m_saveflag = false;
};

#endif // LIVEDATACONSUMER_H
