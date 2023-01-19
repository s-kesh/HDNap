#include "liveimageprocessor.h"

#include <iostream>
#include <chrono>

LiveImageProcessor::LiveImageProcessor(QObject *parent)
    : QObject{parent}
{
}

void LiveImageProcessor::processImage(LiveImage *imgBuffer)
{
    if (m_size != imgBuffer->size) {
        m_size = imgBuffer->size;
        m_sumReal.resize(m_size);
    }

    std::uint64_t index = imgBuffer->index;

    for (size_t i  = 0; i < m_size; ++i) {
        m_sumReal[i] += imgBuffer->data[i];;
     }

    // Need to clamp sum at 255 otherwise it will overflow
    // It is very slow operation
    // need to find some fast way (maybe GPU?)
    for (size_t i = 0; i < m_size; ++i) {
        if (m_sumReal[i] > 255)
            *(m_sum + i) = 255;
        else
            *(m_sum + i) = m_sumReal[i];
    }

    for (size_t i = 0; i < m_size; ++i) {
        *(m_avg + i) = m_sumReal[i] / (index + 1);
    }

    std::uint64_t sumValue = 0;
    for (size_t i  = 0; i < m_size; ++i) {
        sumValue += imgBuffer->data[i];;
     }

    if ((double)sumValue / m_size > m_thres) {
        // It is a hit
        imgBuffer->hit = true;
    }

    emit resultReady(index);
}

void LiveImageProcessor::init(double thres,
                              std::uint8_t *sumbuff,
                              std::uint8_t *avgbuff)
{
    m_thres = thres;
    m_sum = sumbuff;
    m_avg = avgbuff;
}
