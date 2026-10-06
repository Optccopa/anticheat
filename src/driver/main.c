#include <ntifs.h>

#include "callbacks.h"

VOID DriverUnload(IN PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
    NTSTATUS status;
    status = PsRemoveCreateThreadNotifyRoutine(createThreadRoutine);
    if (!NT_SUCCESS(status)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "saturn: failed to unset CreateThreadNotifyRoutine\n");
    }
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "saturn: unloaded\n");
}

NTSTATUS DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
) {
    UNREFERENCED_PARAMETER(RegistryPath);
    
    DriverObject->DriverUnload = DriverUnload;

    NTSTATUS status;
    status = PsSetCreateThreadNotifyRoutine(createThreadRoutine);
    if (!NT_SUCCESS(status)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "saturn: failed to set CreateThreadNotifyRoutine\n");
    }
    
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "saturn: loaded\n");

    return STATUS_SUCCESS;
}