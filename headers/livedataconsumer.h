#ifndef LIVEDATACONSUMER_H
#define LIVEDATACONSUMER_H

#include <QThread>
#include <QSemaphore>
#include "global.h"

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
//    void init (std::uint64_t size,
//               std::uint64_t rollingsize,
//               QPointF *live,
//               QPointF *rollavg,
//               QPointF *avg);

    void init (std::uint64_t size,
               std::uint64_t rollingsize,
               float *x,
               float *y,
               float *rollavg,
               float *avg);

private:
    void _run();

private:
    int m_count;
    bool m_flag = false;
    QSemaphore *m_used;
    QSemaphore *m_free;
    LiveData *m_dataBuffer;

//    std::vector<std::int32_t> m_realSum;
//    std::vector<std::int32_t> m_movingSum;

    MatrixTypef m_realSum;
    MatrixTypef m_movingSum;

    MatrixTypef ivec;

    std::uint64_t m_size = 0;
    std::uint64_t m_rollingsize = 0;

    float *m_x = nullptr;
    float *m_y = nullptr;
    float *m_rollavg = nullptr;
    float *m_avg = nullptr;

//    QPointF *m_live = nullptr;
//    QPointF *m_avg = nullptr;
//    QPointF *m_rollavg = nullptr;

    std::string m_filepath;
    bool m_saveflag = false;

    bool m_requested = false;
};

#endif // LIVEDATACONSUMER_H
