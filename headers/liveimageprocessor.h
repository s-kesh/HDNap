#ifndef LIVEIMAGEPROCESSOR_H
#define LIVEIMAGEPROCESSOR_H

#include "global.h"
#include <QObject>
#include <cstdint>

class LiveImageProcessor : public QObject
{
    Q_OBJECT
public:
    explicit LiveImageProcessor(QObject *parent = nullptr);

signals:
    void resultReady(std::uint64_t index);

public slots:
    void processImage(LiveImage *imgBuffer);
    void init(double thres,
            std::uint8_t *sumbuff,
              std::uint8_t *avgbuff);

private:
    double m_thres = 0.0;
    std::uint64_t m_size = 1;

    std::vector<std::uint32_t> m_sumReal;

    std::uint8_t *m_sum = nullptr;
    std::uint8_t *m_avg = nullptr;
};

#endif // LIVEIMAGEPROCESSOR_H
