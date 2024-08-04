#include "command.h"

#include "odysseus.h"
#include "winservice.h"

Command command_registry[] = {
    {"service:start", command_service_start},
    {"service:stop", command_service_stop},
    {"service:restart", command_service_restart},
    {"service:status", command_service_status},
};

size_t get_command_registry_size() {
    return sizeof(command_registry) / sizeof(command_registry[0]);
}

int command_service_start() {
    SC_HANDLE hSCManager;
    SC_HANDLE hService;
    int return_code = 0;

    if (!service_get_handle(SERVICE_NAME, &hSCManager, &hService)) {
        return_code = service_get_last_error();
        goto end;
    }

    if (!service_start(hService)) {
        return_code = service_get_last_error();
        goto end;
    }

end:
    service_cleanup(hSCManager, hService);
    return return_code;
}

int command_service_stop() {
    SC_HANDLE hSCManager;
    SC_HANDLE hService;
    int return_code = 0;

    if (!service_get_handle(SERVICE_NAME, &hSCManager, &hService)) {
        return_code = service_get_last_error();
        goto end;
    }

    if (!service_stop(hService)) {
        return_code = service_get_last_error();
        goto end;
    }

end:
    service_cleanup(hSCManager, hService);
    return return_code;
}

int command_service_restart() {
    SC_HANDLE hSCManager;
    SC_HANDLE hService;
    int return_code = 0;

    if (!service_get_handle(SERVICE_NAME, &hSCManager, &hService)) {
        return_code = service_get_last_error();
        goto end;
    }

    if (!service_stop(hService)) {
        return_code = service_get_last_error();
        goto end;
    }

    if (!service_start(hService)) {
        return_code = service_get_last_error();
        goto end;
    }

end:
    service_cleanup(hSCManager, hService);
    return return_code;
}

int command_service_status() {
    SC_HANDLE hSCManager;
    SC_HANDLE hService;
    SERVICE_STATUS serviceStatus;
    int return_code = 0;

    if (!service_get_handle(SERVICE_NAME, &hSCManager, &hService)) {
        return_code = service_get_last_error();
        goto end;
    }

    if (!service_status(hService, &serviceStatus)) {
        return_code = service_get_last_error();
        goto end;
    }

    if (serviceStatus.dwCurrentState == SERVICE_RUNNING || serviceStatus.dwCurrentState == SERVICE_START_PENDING) {
        return_code = 1;
        goto end;
    }

end:
    service_cleanup(hSCManager, hService);
    return return_code;
}