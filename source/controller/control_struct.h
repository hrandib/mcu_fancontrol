/*
 * Copyright (c) 2022 Dmytro Shestakov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CONTROL_STRUCT_H
#define CONTROL_STRUCT_H

#include <stdint.h>

#define CRC_INIT_VAL 0xDE
#define CH_MAX_NUMBER 2
#define EMA_MAX 128

struct ControlStruct
{
    enum AlgoType { ALGO_2POINT, ALGO_PI };
    struct PiAlgo
    {
        uint8_t t, kp, ki, max_i;
    };
    struct Point2Algo
    {
        uint8_t tmin, tmax;
    };

    uint8_t pollTimeSecs;

    uint8_t fanStopHysteresis;
    uint8_t pwmMin, pwmMax;

    uint8_t sensorsNumber;
    uint8_t sensorIds[4];

    AlgoType algoType;
    union
    {
        Point2Algo p2Options;
        PiAlgo piOptions;
    } algo;

    uint8_t kEma;

    uint8_t crc;
};

extern const volatile ControlStruct controlStruct[CH_MAX_NUMBER];

#endif // CONTROL_STRUCT_H
