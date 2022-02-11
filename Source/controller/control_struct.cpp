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

#include "control_struct.h"

// clang-format off
// Default config in the eeprom after uploading the firmware
#pragma location = ".eeprom.noinit"
ControlStruct controlStruct[CHANNELS_NUMBER] =
  {
    {   // channel 0
        .pollTimeSecs = 2, // poll time in secs
        .pwmChannel = 0, // PWM channel index (0 or 1)
        .fanStop = false,
        .pwmMin = 25, .pwmMax = 75, // PWM regulation range, corresponds to 0-100%
        .sensorsNumber = 1,
        .sensorIds = { 7, 0, 0, 0 }, //Sensor IDs. Single sensor, LM75 with address 0x07
        .algoType = ControlStruct::ALGO_2POINT, //Algorithm, 2 points
        .algo = {
            .p2Options= { .tmin = 35, .tmax = 55 }
        },
        .crc = 0xAA //Maxim-Dallas 8bit crc, with init = 0xDE
    },
    {   // channel 1
        .pollTimeSecs = 4, // poll time in secs
        .pwmChannel = 1, // PWM channel index (0 or 1)
        .fanStop = false,
        .pwmMin = 10, .pwmMax = 60, // PWM regulation range, corresponds to 0-100%
        .sensorsNumber = 2,
        .sensorIds = { 7, 0x80, 0, 0 }, //Sensor IDs. LM75 with address 0x07 and DS18B20 with id 0
        .algoType = ControlStruct::ALGO_PI, //Algorithm, 2 points
        .algo = {
            .piOptions= {
                .t = 40,  // Temperature set point
                .kp = 16, // Proportional coeff. scaled by 16 (real Kp = kp >> 4)
                .ki = 64, // Integral coeff. scaled by 128 (real Ki = ki >> 7);
                .max_i = 40 // Saturation limit for I value, corresponds to PWM units (0..80)
            }
        },
        .crc = 0x55 //Maxim-Dallas 8bit crc, with init = 0xDE
     }
  };
// clang-format on
