#pragma once

#include <stdint.h>

uint32_t read_message_length();
char* read_message(uint32_t length);
int send_message(int status, const char* format, ...);