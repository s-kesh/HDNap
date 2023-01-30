#ifndef LIVEIMAGECONSUMER_H
#define LIVEIMAGECONSUMER_H

#include <QThread>
#include <QSemaphore>
#include "global.h"

class LiveImageConsumer : public QThread
{
    Q_OBJECT

    void run() override;

public:
    LiveImageConsumer(int count,
                      bool isSaving,
                      QSemaphore *used,
                      QSemaphore *free,
                      LiveImage *imageBuffer,
                      std::string filepath);
    ~LiveImageConsumer();
    void stop();

    bool requested() const;
    void setRequested(bool newRequested);

signals:
    void resultReady(std::uint64_t index);
    void runFinished();

public slots:
    void init(double thres,
              std::uint64_t size,
            std::uint8_t *sumbuff,
              std::uint8_t *avgbuff);

private:
    void _run();

private:
    int m_count;
    bool m_flag = false;
    QSemaphore *m_used;
    QSemaphore *m_free;
    LiveImage *m_imageBuffer;

    double m_thres = 0.0;
    std::uint64_t m_size = 1;

    MatrixTypef m_sumReal;

    std::uint8_t *m_sum = nullptr;
    std::uint8_t *m_avg = nullptr;

    std::string m_filepath;
    bool m_saveflag = false;

    bool m_requested = false;
};

#endif // LIVEIMAGECONSUMER_H
