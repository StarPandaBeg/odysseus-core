#include "winservice.h"

int last_error;
DWORD last_winapi_error;

BOOL check_status(SC_HANDLE hService, DWORD status, BOOL* result) {
    SERVICE_STATUS serviceStatus;
    if (!service_status(hService, &serviceStatus)) {
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

BOOL service_get_handle(const char* serviceName, SC_HANDLE* hSCManager, SC_HANDLE* hService) {
    *hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (*hSCManager == NULL) {
        last_error = ERR_SC_MNG_FAIL;
        last_winapi_error = GetLastError();
        return FALSE;
    }

    *hService = OpenService(*hSCManager, serviceName, SERVICE_ALL_ACCESS);
    if (*hService == NULL) {
        last_error = ERR_SC_FAIL;
        last_winapi_error = GetLastError();
        CloseServiceHandle(*hSCManager);
        return FALSE;
    }

    return TRUE;
}

void service_cleanup(SC_HANDLE hSCManager, SC_HANDLE hService) {
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
}

int service_get_last_error() {
    return last_error;
}

DWORD service_get_last_api_error() {
    return last_winapi_error;
}

BOOL service_status(SC_HANDLE hService, SERVICE_STATUS* status) {
    if (!QueryServiceStatus(hService, status)) {
        last_winapi_error = GetLastError();
        return FALSE;
    }
    return TRUE;
}

BOOL service_start(SC_HANDLE hService) {
    SERVICE_STATUS serviceStatus;
    BOOL isRunning;
    BOOL isStartPending;

    // Do nothing if service is already running
    if (check_status(hService, SERVICE_RUNNING, &isRunning) && isRunning) {
        return TRUE;
    }

    // Wait for service to start if it is already started
    if (check_status(hService, SERVICE_START_PENDING, &isStartPending) && isStartPending) {
        wait_status(hService, SERVICE_START_PENDING, &serviceStatus);
        return TRUE;
    }

    if (!StartService(hService, 0, NULL)) {
        last_error = ERR_SC_ACTION_FAIL;
        return FALSE;
    }

    wait_status(hService, SERVICE_START_PENDING, &serviceStatus);
    if (serviceStatus.dwCurrentState != SERVICE_RUNNING) {
        last_error = ERR_SC_ACTION_FAIL;
        return FALSE;
    }
    return TRUE;
}

BOOL service_stop(SC_HANDLE hService) {
    SERVICE_STATUS serviceStatus;
    BOOL isStopped;
    BOOL isStopPending;

    // Do nothing if service is not running
    if (check_status(hService, SERVICE_STOPPED, &isStopped) && isStopped) {
        return TRUE;
    }

    // Wait for service to stop if it is already queried to stop
    if (check_status(hService, SERVICE_STOP_PENDING, &isStopPending) && isStopPending) {
        wait_status(hService, SERVICE_STOP_PENDING, &serviceStatus);
        return TRUE;
    }

    if (!ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus)) {
        last_error = ERR_SC_ACTION_FAIL;
        return FALSE;
    }

    wait_status(hService, SERVICE_STOP_PENDING, &serviceStatus);
    if (serviceStatus.dwCurrentState != SERVICE_STOPPED) {
        last_error = ERR_SC_ACTION_FAIL;
        return FALSE;
    }
    return TRUE;
}
