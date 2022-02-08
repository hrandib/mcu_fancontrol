#include "shell.h"
#include <cstring>

SHELL_FUNC(DummyFunc)
{
    s.Write("Command not recognized\r\n");
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
