#ifndef SHELL_COMMANDS_H
#define SHELL_COMMANDS_H

#include "base_stream.h"
#include <stdint.h>

#define SHELL_FUNC(NAME) void NAME(BaseStream& s, uint8_t argc, const char* argv[])

typedef void (*CommandFunc)(BaseStream& s, uint8_t argc, const char* argv[]);

struct Command
{
    const char* const cmd_name;
    const CommandFunc cmd_func;
};

extern const Command shell_commands[];

#endif // SHELL_COMMANDS_H
