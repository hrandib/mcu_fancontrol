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

#include "shell.h"
#include <cstring>

SHELL_FUNC(DummyFunc)
{
    ios.Write("Command not recognized\r\n");
}

CommandFunc Shell::GetCommandHandler(const char* cmd)
{
    uint8_t i = 0;
    while(shell_commands[i].cmd_name) {
        if(!strcmp(cmd, shell_commands[i].cmd_name)) {
            return shell_commands[i].cmd_func;
        }
        ++i;
    }
    return DummyFunc;
}

uint8_t Shell::SplitArgs(char* buf, uint8_t length, const char** argv)
{
    uint8_t i = 0;
    argv[i] = buf;
    while(length--) {
        if(*buf == ' ') {
            *buf = '\0';
            if(length) {
                argv[++i] = buf + 1;
            }
        }
        if(i == CMD_MAXARGS - 1) {
            break;
        }
        ++buf;
    }
    return i;
}

void Shell::ProcessCommand(char* buf, uint8_t length)
{
    const char* argv[CMD_MAXARGS];
    uint8_t argc = SplitArgs(buf, length, argv);
    GetCommandHandler(argv[0])(bs_, argc, &argv[1]);
}

void Shell::handle()
{
    uint8_t c;
    while(bs_.Read(&c, 1)) {
        if(c == '\r') {
            cmdBuffer_[charCounter_] = '\0';
            if(charCounter_ >= CMD_MINLENGTH) {
                ProcessCommand(cmdBuffer_, charCounter_);
            }
            charCounter_ = 0;
        }
        else {
            cmdBuffer_[charCounter_++] = c;
            if(charCounter_ == CMD_BUF_SIZE) {
                charCounter_ = 0;
                bs_.Write("Command is too long\r\n");
            }
        }
    }
}
