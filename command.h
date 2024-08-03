#pragma once

#define COMMAND_REGISTRY_SIZE sizeof command_registry / sizeof *command_registry

typedef struct {
    const char* command;
    void (*func)();
} Command;

void test();

Command command_registry[] = {
    {"test", test}};