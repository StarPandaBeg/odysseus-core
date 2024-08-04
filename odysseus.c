/*
 * Odysseus Core - GoodbyeDPI program manager
 * See https://github.com/ValdikSS/GoodbyeDPI
 */

#include "odysseus.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"

int execute_command(char buffer[BUFFER_SIZE], int* exit_status) {
    for (size_t i = 0; i < COMMAND_REGISTRY_SIZE; i++) {
        if (strcmp(buffer, command_registry[i].command) != 0)
            continue;
        *exit_status = command_registry[i].func();
        return 1;
    }
    return 0;
}

int main() {
    char buffer[BUFFER_SIZE];
    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        return ERR_NO_INPUT;
    buffer[strcspn(buffer, "\r\n")] = 0;

    int exit_status;
    if (!execute_command(buffer, &exit_status))
        return ERR_INVALID_COMMAND;
    printf("%d", exit_status);
    return 0;
}