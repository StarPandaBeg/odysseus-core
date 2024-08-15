#include "command.h"

#include <stdio.h>

#include "base64.h"
#include "odysseus.h"

int return_code = 0;

Command command_registry[] = {
    {"service:start", command_service_start},
    {"service:stop", command_service_stop},
    {"service:restart", command_service_restart},
    {"service:status", command_service_status},
    {"service:whitelist", command_read_whitelist},
};

size_t get_command_registry_size() {
    return sizeof(command_registry) / sizeof(command_registry[0]);
}

int get_command_last_status() {
    return return_code;
}

CommandResult command_service_start() {
    SC_HANDLE hSCManager;
    SC_HANDLE hService;
    CommandResult result = COMMAND_RESULT_OK;

    if (!service_get_handle(SERVICE_NAME, &hSCManager, &hService)) {
        goto fail;
    }
    if (!service_start(hService)) {
        goto fail;
    }
    goto end;

fail:
    return_code = service_get_last_error();
    result.status = FALSE;
end:
    service_cleanup(hSCManager, hService);
    return result;
}

CommandResult command_service_stop() {
    SC_HANDLE hSCManager;
    SC_HANDLE hService;
    CommandResult result = COMMAND_RESULT_OK;

    if (!service_get_handle(SERVICE_NAME, &hSCManager, &hService)) {
        goto fail;
    }
    if (!service_stop(hService)) {
        goto fail;
    }
    goto end;

fail:
    return_code = service_get_last_error();
    result.status = FALSE;
end:
    service_cleanup(hSCManager, hService);
    return result;
}

CommandResult command_service_restart() {
    SC_HANDLE hSCManager;
    SC_HANDLE hService;
    CommandResult result = COMMAND_RESULT_OK;

    if (!service_get_handle(SERVICE_NAME, &hSCManager, &hService)) {
        goto fail;
    }
    if (!service_stop(hService)) {
        goto fail;
    }
    if (!service_start(hService)) {
        goto fail;
    }
    goto end;

fail:
    return_code = service_get_last_error();
    result.status = FALSE;
end:
    service_cleanup(hSCManager, hService);
    return result;
}

CommandResult command_service_status() {
    SC_HANDLE hSCManager;
    SC_HANDLE hService;
    SERVICE_STATUS serviceStatus;
    CommandResult result = COMMAND_RESULT_OK;
    BOOL isOnline = FALSE;

    if (!service_get_handle(SERVICE_NAME, &hSCManager, &hService)) {
        goto fail;
    }
    if (!service_status(hService, &serviceStatus)) {
        goto fail;
    }

    if (serviceStatus.dwCurrentState == SERVICE_RUNNING || serviceStatus.dwCurrentState == SERVICE_START_PENDING) {
        isOnline = TRUE;
    }
    result.data = isOnline ? "1" : "0";
    goto end;

fail:
    return_code = service_get_last_error();
    result.status = FALSE;
end:
    service_cleanup(hSCManager, hService);
    return result;
}

CommandResult command_read_whitelist() {
    CommandResult result = COMMAND_RESULT_OK;
    FILE* file = fopen(WHITELIST_PATH, "r");
    if (file == NULL) {
        return_code = ERR_FILE_FAIL;
        goto fail;
    }

    fseek(file, 0, SEEK_END);
    size_t fileSize = (size_t)ftell(file);
    rewind(file);

    unsigned char* content = (unsigned char*)malloc((fileSize + 1) * sizeof(char));
    if (content == NULL) {
        return_code = ERR_FILE_ALLOC_FAIL;
        goto fail;
    }

    size_t readSize = fread(content, sizeof(char), (size_t)fileSize, file);
    content[readSize] = '\0';

    size_t encodedSize;
    char* encoded = base64_encode(content, readSize, &encodedSize);
    result.data = encoded;

    free(content);
    goto end;

fail:
    result.status = FALSE;
end:
    fclose(file);
    return result;
}
