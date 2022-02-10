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
        .pollTimeSecs = 4, // poll time in secs
        .pwmChannel = 0, // PWM channel index (0 or 1)
        .pwmMin = 0, .pwmMax = 80, // PWM regulation range, corresponds to 0-100%
        .sensorsNumber = 1,
        .sensorIds = { 7, 0, 0, 0 }, //Sensor IDs. Single sensor, LM75 with address 0x07
        .algoType = ControlStruct::ALGO_2POINT, //Algorithm, 2 points
        .algo = {
            .p2Options= { .tmin = 45, .tmax = 65 }
        },
        .crc = 0xCE //Maxim-Dallas 8bit crc, with init = 0xDE
    },
    {   // channel 1
        .pollTimeSecs = 4, // poll time in secs
        .pwmChannel = 1, // PWM channel index (0 or 1)
        .pwmMin = 0, .pwmMax = 60, // PWM regulation range, corresponds to 0-100%
        .sensorsNumber = 1,
        .sensorIds = { 7, 0, 0, 0 }, //Sensor IDs. Single sensor, LM75 with address 0x07
        .algoType = ControlStruct::ALGO_2POINT, //Algorithm, 2 points
        .algo = {
            .p2Options= { .tmin = 45, .tmax = 75 }
        },
        .crc = 0xE2 //Maxim-Dallas 8bit crc, with init = 0xDE
     }
  };
// clang-format on
