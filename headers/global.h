#ifndef GLOBAL_H
#define GLOBAL_H

#include <cmath>
#include <cstdint>
#include <vector>

#define BUFFSIZE 500

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
