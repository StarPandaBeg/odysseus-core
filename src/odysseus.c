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

void send_result_message(CommandResult* result) {
    if (result->data == NULL) {
        send_message(result->status, "");
        return;
    }
    send_message(result->status, "%s", result->data);
}

int execute_command(char buffer[BUFFER_SIZE], CommandResult* result) {
    for (size_t i = 0; i < COMMAND_REGISTRY_SIZE; i++) {
        if (strcmp(buffer, command_registry[i].command) != 0)
            continue;
        *result = command_registry[i].func();
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

    CommandResult result;
    if (!execute_command(message, &result))
        return ERR_INVALID_COMMAND;
    send_result_message(&result);
    return 0;
}