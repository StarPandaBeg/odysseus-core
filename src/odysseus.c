/*
 * Odysseus Core - GoodbyeDPI program manager
 * See https://github.com/ValdikSS/GoodbyeDPI
 */

#include "odysseus.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "command.h"
#include "messaging.h"

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
    _setmode(_fileno(stdin), O_BINARY);
    _setmode(_fileno(stdout), O_BINARY);

    uint32_t message_size = read_message_length();
    if (!message_size)
        return ERR_NO_INPUT;
    char* message = read_message(message_size);

    int exit_status;
    if (!execute_command(message, &exit_status))
        return ERR_INVALID_COMMAND;
    send_message("%d", exit_status);
    return 0;
}