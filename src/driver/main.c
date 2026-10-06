#include <ntifs.h>

#include "callbacks.h"

VOID DriverUnload(IN PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);

    NTSTATUS threadStatus;
    threadStatus = PsRemoveCreateThreadNotifyRoutine(createThreadRoutine);
    if (!NT_SUCCESS(threadStatus)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "saturn: failed to unset CreateThreadNotifyRoutine, err: %lu\n", threadStatus);
    }

    NTSTATUS processStatus;
    processStatus = PsSetCreateProcessNotifyRoutineEx(createProcessRoutine, TRUE);
    if (!NT_SUCCESS(processStatus)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "saturn: failed to unset CreateProcessNotifyRoutineEx, err: %lu\n", processStatus);
    }

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "saturn: unloaded\n");
}

NTSTATUS DriverEntry(
    IN PDRIVER_OBJECT  DriverObject,
    IN PUNICODE_STRING RegistryPath
) {
    UNREFERENCED_PARAMETER(RegistryPath);
    
    DriverObject->DriverUnload = DriverUnload;

    NTSTATUS threadStatus;
    threadStatus = PsSetCreateThreadNotifyRoutine(createThreadRoutine);
    if (!NT_SUCCESS(threadStatus)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "saturn: failed to set CreateThreadNotifyRoutine, err: %ld\n", threadStatus);
    }

    NTSTATUS processStatus;
    processStatus = PsSetCreateProcessNotifyRoutineEx(createProcessRoutine, FALSE);
    if (!NT_SUCCESS(threadStatus)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "saturn: failed to set CreateProcessNotifyRoutineEx, err: %ld\n", processStatus);
    }
    
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "saturn: loaded\n");

    return STATUS_SUCCESS;
}