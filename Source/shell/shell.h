#ifndef SHELL_H
#define SHELL_H

#include "base_stream.h"
#include "scm_utils.h"
#include "shell_commands.h"
#include "shell_config.h"

class Shell
{
public:
    Shell(BaseStream& stream) : bs_(stream), charCounter_()
    {
        bs_.Write("[ FAN CONTROLLER SHELL ]\r\n");
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
