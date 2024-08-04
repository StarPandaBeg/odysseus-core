#include <stdio.h>
#include <windows.h>

#include "odysseus.h"
#include "util.h"

int last_error;

SC_HANDLE get_service_handler(DWORD desiredAccess) {
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCManager == NULL) {
        last_error = print_error_ext(-1, "error:sc_manager_fail:%lu", GetLastError());
        return NULL;
    }

    SC_HANDLE hService = OpenService(hSCManager, SERVICE_NAME, desiredAccess);
    if (hService == NULL) {
        CloseServiceHandle(hSCManager);
        last_error = print_error_ext(-2, "error:service_fail:%lu", GetLastError());
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

int service_stop() {
    SC_HANDLE hService = get_service_handler(SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (hService == NULL)
        return last_error;

    SERVICE_STATUS serviceStatus;
    if (QueryServiceStatus(hService, &serviceStatus)) {
        if (serviceStatus.dwCurrentState == SERVICE_STOPPED)
            return print_error(0, "ok");
    }

    if (!ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus)) {
        return print_error_ext(-3, "error:service_stop_fail:%lu", GetLastError());
    }

    wait_status(hService, SERVICE_STOP_PENDING, &serviceStatus);
    if (serviceStatus.dwCurrentState != SERVICE_STOPPED) {
        return print_error_ext(-3, "error:service_stop_fail:%lu", GetLastError());
    }

    return print_error(0, "ok");
}
