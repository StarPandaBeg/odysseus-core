#include "messaging.h"

#define RESPONSE_FORMAT "{\"response\": \"%s\"}"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *to_json_response(char *message) {
    int json_size = snprintf(NULL, 0, RESPONSE_FORMAT, message) + 1;
    char *json_buffer = (char *)malloc((size_t)json_size);
    if (!json_buffer) {
        free(message);
        return NULL;
    }
    snprintf(json_buffer, (size_t)json_size, RESPONSE_FORMAT, message);
    return json_buffer;
}

uint32_t read_message_length() {
    uint32_t length = 0;
    if (!fread(&length, sizeof(uint32_t), 1, stdin)) {
        return 0;
    }
    return length;
}

char *read_message(uint32_t length) {
    char quote_buf;
    char *message = (char *)malloc(length - 2);

    fread(&quote_buf, sizeof(char), 1, stdin);  // Read opening quote (")
    fread(message, sizeof(char), length - 2, stdin);
    fread(&quote_buf, sizeof(char), 1, stdin);  // Read ending quote (")

    message[length - 2] = '\0';
    return message;
}

int send_message(const char *format, ...) {
    char *buffer = NULL;
    int buffer_size = 0;
    va_list args;

    // Calculate buffer size for raw message
    va_start(args, format);
    buffer_size = vsnprintf(NULL, 0, format, args) + 1;  // +1 for '\0'
    va_end(args);

    // Allocate buffer for raw message
    buffer = (char *)malloc((size_t)buffer_size);
    if (!buffer) {
        return -1;
    }

    // Format message
    va_start(args, format);
    vsnprintf(buffer, (size_t)buffer_size, format, args);
    va_end(args);

    // Create JSON response
    char *response = to_json_response(buffer);
    uint32_t json_size = (uint32_t)strlen(response);

    // Write message to stdout preceded with an unsigned 32-bit message length
    fwrite(&json_size, sizeof(uint32_t), 1, stdout);
    fwrite(response, sizeof(char), json_size, stdout);
    fflush(stdout);

    free(buffer);
    free(response);
    return (int)json_size + 1;
}