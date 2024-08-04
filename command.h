#pragma once

#define COMMAND_REGISTRY_SIZE sizeof command_registry / sizeof *command_registry

typedef struct Command {
    const char* command;
    int (*func)();
} Command;

int service_stop();

Command command_registry[] = {
    {"service:stop", service_stop}};