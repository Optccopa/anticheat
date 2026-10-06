#include <ntifs.h>

static const UNICODE_STRING cs2Suffix = RTL_CONSTANT_STRING(L"\\game\\bin\\win64\\cs2.exe");
static volatile HANDLE g_InitialThreadSeenPid = NULL;
static volatile HANDLE g_Cs2Pid = NULL;
static volatile LONG g_ExpectInitialThread = FALSE;

VOID createProcessRoutine(
    IN PEPROCESS Process,
    IN HANDLE ProcessId,
    IN PPS_CREATE_NOTIFY_INFO CreateInfo) {
    UNREFERENCED_PARAMETER(Process);

    if (CreateInfo == NULL) { // exit
        if (ProcessId == g_Cs2Pid) {
            InterlockedExchangePointer((PVOID volatile*)&g_Cs2Pid, NULL);
        }
        return;
    }

    if (CreateInfo->ImageFileName == NULL) {
        return;
    }

    if (!RtlSuffixUnicodeString(&cs2Suffix, (PUNICODE_STRING)CreateInfo->ImageFileName, TRUE)) {
        return;
    }

    InterlockedExchange(&g_ExpectInitialThread, TRUE);
    InterlockedExchangePointer((PVOID volatile*)&g_Cs2Pid, ProcessId);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
        "saturn: cs2 launched, pid %lu, path %wZ\n",
        (ULONG)(ULONG_PTR)ProcessId, CreateInfo->ImageFileName
    );

    InterlockedExchangePointer(&g_InitialThreadSeenPid, NULL);
}

VOID createThreadRoutine(
    IN HANDLE ProcessId,
    IN HANDLE ThreadId,
    IN BOOLEAN Created
) {
    if (Created) { 
        UNREFERENCED_PARAMETER(ThreadId);
        if (ProcessId == g_Cs2Pid) {
            HANDLE currentProcessId = PsGetCurrentProcessId();
            if (currentProcessId != ProcessId) {
                // BUG: if the driver loads after cs2 starts
                // the first legit remote thread gets ignored.
                if (g_InitialThreadSeenPid != ProcessId) {
                    InterlockedExchangePointer(&g_InitialThreadSeenPid, ProcessId);
                    return;
                }

                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "saturn: detected external thread in cs2 process, parent: %lu\n", currentProcessId);
            }
            
        }
    }
}
