#pragma once

#include <windows.h>

#define ERR_SC_MNG_FAIL -1
#define ERR_SC_FAIL -2
#define ERR_SC_ACTION_FAIL -3

BOOL service_get_handle(const char* serviceName, SC_HANDLE* hSCManager, SC_HANDLE* hService);
void service_cleanup(SC_HANDLE hSCManager, SC_HANDLE hService);

int service_get_last_error();
DWORD service_get_last_api_error();

BOOL service_status(SC_HANDLE hService, SERVICE_STATUS* status);
BOOL service_start(SC_HANDLE hService);
BOOL service_stop(SC_HANDLE hService);