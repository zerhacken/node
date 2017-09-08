#include "node.h"

#include <stdio.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Dbghelp.h>

namespace node {

void DumpBacktrace(FILE* fp) {

    typedef USHORT (*RtlCaptureStackBackTraceType)(_In_ ULONG FramesToSkip, _In_ ULONG FramesToCapture, _Out_ PVOID *BackTrace, _Out_opt_ PULONG BackTraceHash);
    static RtlCaptureStackBackTraceType RtlCaptureStackBackTrace = 0;
    if (RtlCaptureStackBackTrace == 0)
    {
        HMODULE hModule = LoadLibraryA("Kernel32.dll");
        RtlCaptureStackBackTrace = (RtlCaptureStackBackTraceType)GetProcAddress(hModule, "RtlCaptureStackBackTrace");
    }

    void* stack[256];

    HANDLE hProcess = GetCurrentProcess();
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms681351(v=vs.85).aspx
    if (SymInitialize(hProcess, NULL, TRUE) != TRUE)
    {
        // TODO: handle using GetLastError() ?
    }

    const int frames = CaptureStackBackTrace(0, 256, stack, NULL);
    SYMBOL_INFO* symbol = (SYMBOL_INFO*) malloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char));
    symbol->MaxNameLen   = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    const int skip = 1;
    for (int i = skip; i < frames; i += 1)
    {
        // https://msdn.microsoft.com/en-us/library/windows/desktop/ms681323(v=vs.85).aspx
        SymFromAddr(hProcess, (DWORD64)stack[i], 0, symbol);
        fprintf(fp, "%2d: %s\n", i, symbol->Name);
    }
    free(symbol);
}

}  // namespace node
