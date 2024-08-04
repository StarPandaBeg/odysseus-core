#include "command.h"

#include <stdio.h>
#include <windows.h>

#include "odysseus.h"

int last_error;

Command command_registry[] = {
    {"service:stop", service_stop}};

SC_HANDLE get_service_handler(DWORD desiredAccess) {
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCManager == NULL) {
        last_error = ERR_SC_MNG_FAIL;
        return NULL;
    }

    SC_HANDLE hService = OpenService(hSCManager, SERVICE_NAME, desiredAccess);
    if (hService == NULL) {
        CloseServiceHandle(hSCManager);
        last_error = ERR_SC_FAIL;
        return NULL;
    }
    return hService;
}

void wait_status(SC_HANDLE hService, DWORD status, SERVICE_STATUS* outStatus) {
    while (QueryServiceStatus(hService, outStatus)) {
        if (outStatus->dwCurrentState == status) {
            Sleep(1000);
        }
        break;
    }
}

size_t get_command_registry_size() {
    return sizeof(command_registry) / sizeof(command_registry[0]);
}

int service_stop() {
    SC_HANDLE hService = get_service_handler(SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (hService == NULL)
        return last_error;

    SERVICE_STATUS serviceStatus;
    if (QueryServiceStatus(hService, &serviceStatus)) {
        if (serviceStatus.dwCurrentState == SERVICE_STOPPED)
            return RESULT_OK;
    }

    if (!ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus)) {
        return ERR_SC_ACTION_FAIL;
    }

    wait_status(hService, SERVICE_STOP_PENDING, &serviceStatus);
    if (serviceStatus.dwCurrentState != SERVICE_STOPPED) {
        return ERR_SC_ACTION_FAIL;
    }

    return RESULT_OK;
}
