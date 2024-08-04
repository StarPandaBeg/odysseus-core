#include <stdarg.h>
#include <stdio.h>

int print_error(int code, const char* message) {
    printf("%s", message);
    return code;
}

int print_error_ext(int code, const char* format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    return code;
}