#include "command.h"

#include "odysseus.h"

int return_code = 0;

Command command_registry[] = {
    {"service:start", command_service_start},
    {"service:stop", command_service_stop},
    {"service:restart", command_service_restart},
    {"service:status", command_service_status},
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

    if (!service_get_handle(SERVICE_NAME, &hSCManager, &hService)) {
        goto fail;
    }
    if (!service_status(hService, &serviceStatus)) {
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