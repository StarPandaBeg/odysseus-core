#pragma once

#include <stddef.h>

#define COMMAND_REGISTRY_SIZE get_command_registry_size()

typedef struct Command {
    const char* command;
    int (*func)();
} Command;

extern Command command_registry[];
size_t get_command_registry_size();

int command_service_start();
int command_service_stop();
int command_service_restart();
