#pragma once

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

DWORD startDriver(const wchar_t *sysPath) {
    SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (!scm) return GetLastError();
    SC_HANDLE svc = OpenServiceW(scm, L"SaturnAC", SERVICE_ALL_ACCESS);
    if (!svc)
        svc = CreateServiceW(scm, L"SaturnAC", L"SaturnAC",
            SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER,
            SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
            sysPath, NULL, NULL, NULL, NULL, NULL);
    DWORD err = 0;
    if (!svc || !StartServiceW(svc, 0, NULL)) err = GetLastError();
    if (err == ERROR_SERVICE_ALREADY_RUNNING) err = 0;
    if (svc) CloseServiceHandle(svc);
    CloseServiceHandle(scm);
    return err;
}

void stopDriver(void) {
    SC_HANDLE scm = OpenSCManagerW(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    SC_HANDLE svc = OpenServiceW(scm, L"SaturnAC", SERVICE_ALL_ACCESS);
    SERVICE_STATUS status;
    ControlService(svc, SERVICE_CONTROL_STOP, &status);
    DeleteService(svc);
    CloseServiceHandle(svc);
    CloseServiceHandle(scm);
}
