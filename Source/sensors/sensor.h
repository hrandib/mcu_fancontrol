#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

struct Sensor
{
    virtual uint8_t GetTemp() = 0;
    virtual uint8_t GetId() = 0;
};

#endif // SENSOR_H
