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

#ifndef SHELL_H
#define SHELL_H

#include "base_stream.h"
#include "scm_utils.h"
#include "sensor_handler.h"
#include "shell_commands.h"
#include "shell_config.h"

class Shell
{
public:
    Shell(BaseStream& stream) : bs_(stream), charCounter_()
    {
        bs_.Write("\r\n[ FAN CONTROLLER SHELL ]\r\n");
    }
    void handle();
    uint8_t SplitArgs(char* buf, uint8_t bufLength, const char** argv);
private:
    BaseStream& bs_;
    uint8_t charCounter_;
    char cmdBuffer_[CMD_BUF_SIZE];

    void ProcessCommand(char* buf, uint8_t length);
    CommandFunc GetCommandHandler(const char* cmd);
};

#endif // SHELL_H
