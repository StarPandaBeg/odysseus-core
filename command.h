#pragma once

#include <stddef.h>

#define COMMAND_REGISTRY_SIZE get_command_registry_size()

#define RESULT_OK 0
#define ERR_SC_MNG_FAIL -1
#define ERR_SC_FAIL -2
#define ERR_SC_ACTION_FAIL -3

typedef struct Command {
    const char* command;
    int (*func)();
} Command;

extern Command command_registry[];
size_t get_command_registry_size();

int service_stop();
