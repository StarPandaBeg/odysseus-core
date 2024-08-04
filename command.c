#include "command.h"

#include <stdio.h>
#include <windows.h>

#include "odysseus.h"

int last_error;
DWORD last_winapi_error;

Command command_registry[] = {
    {"service:stop", service_stop}};

BOOL get_service_handler(DWORD desiredAccess, SC_HANDLE* hSCManager, SC_HANDLE* hService) {
    *hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (*hSCManager == NULL) {
        last_error = ERR_SC_MNG_FAIL;
        last_winapi_error = GetLastError();
        return FALSE;
    }

    *hService = OpenService(*hSCManager, SERVICE_NAME, desiredAccess);
    if (*hService == NULL) {
        last_error = ERR_SC_FAIL;
        last_winapi_error = GetLastError();
        CloseServiceHandle(*hSCManager);
        return FALSE;
    }

    return TRUE;
}

BOOL check_status(SC_HANDLE hService, DWORD status, BOOL* result) {
    SERVICE_STATUS serviceStatus;
    if (!QueryServiceStatus(hService, &serviceStatus)) {
        return FALSE;
    }
    *result = (serviceStatus.dwCurrentState == status);
    return TRUE;
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
    SC_HANDLE hSCManager;
    SC_HANDLE hService;
    int return_code = RESULT_OK;

    if (!get_service_handler(SERVICE_STOP | SERVICE_QUERY_STATUS, &hSCManager, &hService)) {
        return_code = last_error;
        goto end;
    }

    SERVICE_STATUS serviceStatus;
    BOOL isStopped;
    if (check_status(hService, SERVICE_STOPPED, &isStopped) && isStopped) {
        goto end;
    }

    if (!ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus)) {
        return_code = ERR_SC_ACTION_FAIL;
        goto end;
    }

    wait_status(hService, SERVICE_STOP_PENDING, &serviceStatus);
    if (serviceStatus.dwCurrentState != SERVICE_STOPPED) {
        return_code = ERR_SC_ACTION_FAIL;
        goto end;
    }

end:
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return return_code;
}
