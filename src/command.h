#pragma once

#include <stddef.h>

#include "winservice.h"

#define COMMAND_REGISTRY_SIZE get_command_registry_size()
#define COMMAND_RESULT_OK {TRUE, NULL}

typedef struct {
    BOOL status;
    void* data;
} CommandResult;

typedef struct Command {
    const char* command;
    CommandResult (*func)();
} Command;

extern Command command_registry[];
size_t get_command_registry_size();
int get_command_last_status();

CommandResult command_service_start();
CommandResult command_service_stop();
CommandResult command_service_restart();
CommandResult command_service_status();
