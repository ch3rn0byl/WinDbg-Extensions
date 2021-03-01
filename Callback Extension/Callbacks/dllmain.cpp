// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

EXTERN_C
__declspec(dllexport)
HRESULT CALLBACK DebugExtensionInitialize(
    _In_ PULONG Version,
    _In_ PULONG
)
{
    *Version = DEBUG_EXTENSION_VERSION(MAJOR, MINOR);

    return init();
}

EXTERN_C
__declspec(dllexport)
void CALLBACK DebugExtensionNotify(
    _In_ ULONG,
    _In_ ULONG64
)
{
    return;
}

EXTERN_C
__declspec(dllexport)
void CALLBACK DebugExtensionUninitialize()
{
    cleanUpAndExit();
    return;
}


/// EOF