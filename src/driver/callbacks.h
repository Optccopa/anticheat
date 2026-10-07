#include <ntifs.h>

static const UNICODE_STRING cs2Suffix = RTL_CONSTANT_STRING(L"\\game\\bin\\win64\\cs2.exe");
static volatile HANDLE g_Cs2Pid = NULL;

#ifndef THREAD_QUERY_INFORMATION
#define THREAD_QUERY_INFORMATION (0x0040)
#endif

#ifndef MEM_IMAGE
#define MEM_IMAGE 0x1000000
#endif

NTSYSAPI NTSTATUS NTAPI ZwQueryInformationThread(
    IN HANDLE ThreadHandle,
    IN THREADINFOCLASS ThreadInformationClass,
    OUT PVOID ThreadInformation,
    IN ULONG ThreadInformationLength,
    OUT PULONG ReturnLength OPTIONAL);

VOID createProcessRoutine(
    IN PEPROCESS Process,
    IN HANDLE ProcessId,
    IN PPS_CREATE_NOTIFY_INFO CreateInfo) {
    UNREFERENCED_PARAMETER(Process);

    if (CreateInfo == NULL) { // exit
        if (ProcessId == g_Cs2Pid) {
            InterlockedExchangePointer((PVOID volatile*)&g_Cs2Pid, NULL);
            DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                "saturn: cs2 stopped, pid %lu\n", (ULONG)(ULONG_PTR)ProcessId
            );
        }
        return;
    }

    if (CreateInfo->ImageFileName == NULL) {
        return;
    }

    if (!RtlSuffixUnicodeString(&cs2Suffix, (PUNICODE_STRING)CreateInfo->ImageFileName, TRUE)) {
        return;
    }

    InterlockedExchangePointer((PVOID volatile*)&g_Cs2Pid, ProcessId);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
        "saturn: cs2 launched, pid %lu\n", (ULONG)(ULONG_PTR)ProcessId
    );
}

// ISSUE: if the driver loads after cs2 starts
// the first legit remote thread gets logged
VOID createThreadRoutine(
    IN HANDLE ProcessId,
    IN HANDLE ThreadId,
    IN BOOLEAN Created
) {
    if (Created) { 
        if (ProcessId == g_Cs2Pid) {
            HANDLE currentProcessId = PsGetCurrentProcessId();
            if (currentProcessId != ProcessId) {
                PETHREAD thread;
                if (!NT_SUCCESS(PsLookupThreadByThreadId(ThreadId, &thread))) {
                    return;
                }

                HANDLE threadHandle;
                NTSTATUS status = ObOpenObjectByPointer(thread, OBJ_KERNEL_HANDLE, NULL,
                    THREAD_QUERY_INFORMATION, *PsThreadType, KernelMode, &threadHandle);

                if (NT_SUCCESS(status)) {
                    PVOID startAddress = NULL;

                    status = ZwQueryInformationThread(threadHandle, ThreadQuerySetWin32StartAddress,
                        &startAddress, sizeof(startAddress), NULL);

                    ZwClose(threadHandle);

                    if (NT_SUCCESS(status)) {
                        MEMORY_BASIC_INFORMATION mbi;
                        KAPC_STATE apcState;
                        KeStackAttachProcess(PsGetThreadProcess(thread), &apcState);

                        status = ZwQueryVirtualMemory(ZwCurrentProcess(), startAddress,
                            MemoryBasicInformation, &mbi, sizeof(mbi), NULL);

                        KeUnstackDetachProcess(&apcState);
                        if (NT_SUCCESS(status)) {
                            if (mbi.Type != MEM_IMAGE) {
                                DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
                                    "saturn: suspicous thread %lu starts outside an image at %p (type 0x%lx, protect 0x%lx)\n",
                                    (ULONG)(ULONG_PTR)ThreadId, startAddress, mbi.Type, mbi.Protect,
                                    (ULONG)(ULONG_PTR)PsGetCurrentProcessId());
                            }                        
                        }

                    }
                }
                ObDereferenceObject(thread);
            }
        }
    }
}

VOID loadImageRoutine(
    IN PUNICODE_STRING FullImageName,
    IN HANDLE ProcessId,
    IN PIMAGE_INFO ImageInfo
) {
    if (ProcessId == NULL) { return; } // Kernel driver

    if (FullImageName && ProcessId == g_Cs2Pid) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL,
            "saturn: process %p loaded image: %wZ at Address: %p\n", 
            ProcessId, FullImageName, ImageInfo->ImageBase
        );
    }
}