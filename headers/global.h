#ifndef GLOBAL_H
#define GLOBAL_H

#include <cmath>
#include <cstdint>
#include <Eigen/Dense>
#define BUFFSIZE 500

typedef Eigen::Matrix<std::uint32_t, 1, Eigen::Dynamic> MatrixTypeU32;
typedef Eigen::Map<MatrixTypeU32> MapTypeU32;
typedef Eigen::Map<const MatrixTypeU32> MapTypeConstU32;
typedef Eigen::Matrix<std::uint16_t, 1, Eigen::Dynamic> MatrixTypeU16;
typedef Eigen::Map<MatrixTypeU16> MapTypeU16;
typedef Eigen::Map<const MatrixTypeU16> MapTypeConstU16;
typedef Eigen::Matrix<std::uint8_t, 1, Eigen::Dynamic> MatrixTypeU8;
typedef Eigen::Map<MatrixTypeU8> MapTypeU8;
typedef Eigen::Map<const MatrixTypeU8> MapTypeConstU8;

typedef Eigen::Matrix<std::int32_t, 1, Eigen::Dynamic> MatrixType32;
typedef Eigen::Map<MatrixType32> MapType32;
typedef Eigen::Map<const MatrixType32> MapTypeConst32;
typedef Eigen::Matrix<std::int16_t, 1, Eigen::Dynamic> MatrixType16;
typedef Eigen::Map<MatrixType16> MapType16;
typedef Eigen::Map<const MatrixType16> MapTypeConst16;
typedef Eigen::Matrix<std::int8_t, 1, Eigen::Dynamic> MatrixType8;
typedef Eigen::Map<MatrixType8> MapType8;
typedef Eigen::Map<const MatrixType8> MapTypeConst8;


typedef Eigen::Matrix<float, 1, Eigen::Dynamic> MatrixTypef;
typedef Eigen::Map<MatrixTypef> MapTypef;
typedef Eigen::Map<const MatrixTypef> MapTypeConstf;
typedef Eigen::Matrix<float, 1, Eigen::Dynamic> MatrixTypef;
typedef Eigen::Map<MatrixTypef> MapTypef;
typedef Eigen::Map<const MatrixTypef> MapTypeConstf;
typedef Eigen::Matrix<float, 1, Eigen::Dynamic> MatrixTypef;
typedef Eigen::Map<MatrixTypef> MapTypf;
typedef Eigen::Map<const MatrixTypef> MapTypeConsf;

struct LiveImage
{
    bool hit = false;
    std::uint64_t index = 0;
    std::uint64_t id = 0;
    std::uint64_t timestamp = 0;
    std::uint64_t size = 0;
    std::uint8_t *data = nullptr;
};

struct LiveData
{
    bool hit = false;
    std::double_t timeoffset = 0;
    std::double_t timefactor = 0;
    std::double_t voltoffset = 0;
    std::double_t voltfactor = 0;
    std::double_t timestampseconds = 0;
    std::double_t timestampsfractions = 0;
    std::int64_t size = 0;
    std::int64_t firsttimepoint = 0;
    std::int64_t index = 0;
    std::int16_t *data = nullptr;
};

#endif // GLOBAL_H
