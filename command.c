#include "command.h"

#include "odysseus.h"
#include "winservice.h"

Command command_registry[] = {
    {"service:start", command_service_start},
    {"service:stop", command_service_stop},
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
