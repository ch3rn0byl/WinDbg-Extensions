// callbacks.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "callbacks.h"

WINDBG_EXTENSION_APIS ExtensionApis;

PIDebugClient g_pDebugClient = nullptr;
PIDebugControl g_pDebugControl = nullptr;
PIDebugSymbols g_pSymbols = nullptr;

/// <summary>
/// Init will intiate the global variables used by this extension to use printing, symbols, etc.
/// </summary>
/// <returns>S_OK if successful</returns>
HRESULT init()
{
    HRESULT hResult = DebugCreate(__uuidof(IDebugClient), reinterpret_cast<PVOID*>(&g_pDebugClient));
    if (FAILED(hResult))
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_ERROR, "[%ws::%d] Failed with status: %d\n", __FUNCTIONW__, __LINE__, hResult);
        return hResult;
    }

    hResult = g_pDebugClient->QueryInterface(__uuidof(IDebugControl), reinterpret_cast<PVOID*>(&g_pDebugControl));
    if (FAILED(hResult))
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_ERROR, "[%ws::%d] Failed with status: %d\n", __FUNCTIONW__, __LINE__, hResult);
        return hResult;
    }

    ExtensionApis.nSize = sizeof(ExtensionApis);
    hResult = g_pDebugControl->GetWindbgExtensionApis64(&ExtensionApis);
    if (FAILED(hResult))
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_ERROR, "[%ws::%d] Failed with status: %d\n", __FUNCTIONW__, __LINE__, hResult);
        return hResult;
    }

    hResult = g_pDebugClient->QueryInterface(__uuidof(IDebugSymbols), reinterpret_cast<PVOID*>(&g_pSymbols));
    if (FAILED(hResult))
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_ERROR, "[%ws::%d] Failed with status: %d\n", __FUNCTIONW__, __LINE__, hResult);
        return hResult;
    }
    return hResult;
}

/// <summary>
/// This will enumerate all Load Image Routine callbacks registered on a system. It will show the pointer
/// that it's located at, as well as the function of that callback. 
/// </summary>
/// <returns>S_OK if successful</returns>
HRESULT enumerateLoadImageRoutines()
{
    std::string SymbolName(MAX_SYMBOL_LENGTH, '\0');
    std::ostringstream ss;

    ULONG64 PspLoadImageNotifyRoutine = 0;
    ULONG64 PspLoadImageNotifyRoutineCount = 0;

    ULONG64 LoadImageRoutineCount = 0;
    ULONG64 temp = 0;

    HRESULT hResult = g_pSymbols->GetOffsetByName("nt!PspLoadImageNotifyRoutine", &PspLoadImageNotifyRoutine);
    if (FAILED(hResult))
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_ERROR, "[%ws::%d] Failed with status: %d\n", __FUNCTIONW__, __LINE__, hResult);
        return hResult;
    }

    hResult = g_pSymbols->GetOffsetByName("nt!PspLoadImageNotifyRoutineCount", &PspLoadImageNotifyRoutineCount);
    if (FAILED(hResult))
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_ERROR, "[%ws::%d] Failed with status: %d\n", __FUNCTIONW__, __LINE__, hResult);
        return hResult;
    }

    //
    // Get the total count for PspLoadImageNotifyRoutineCount
    //
    ReadPointer(PspLoadImageNotifyRoutineCount, &LoadImageRoutineCount);

    g_pDebugControl->Output(DEBUG_OUTPUT_DEBUGGEE, "The following Load Image Notify routines are registered on the system:\n");
    for (int i = 0; i < LoadImageRoutineCount; i++)
    {
        UINT64 ImageCallbackPointer = 0;
        UINT64 ImageCallbackAddress = 0;

        ULONG64 Displacement = 0;

        ReadPointer(PspLoadImageNotifyRoutine, &ImageCallbackPointer);
        ImageCallbackPointer &= ~15;
        ImageCallbackPointer += sizeof(ImageCallbackPointer);

        ReadPointer(ImageCallbackPointer, &ImageCallbackAddress);

        hResult = g_pSymbols->GetNameByOffset(ImageCallbackAddress, &SymbolName[0], static_cast<ULONG>(SymbolName.size()), NULL, &Displacement);
        if (FAILED(hResult))
        {
            return hResult;
        }

        if (Displacement != 0)
        {
            ss << std::hex << "+" << Displacement;

            SymbolName.insert(strlen(SymbolName.c_str()), ss.str().c_str());

            ss.str(std::string());
        }

        g_pDebugControl->Output(DEBUG_OUTPUT_DEBUGGEE, "%p: %s\n", ImageCallbackPointer, SymbolName.c_str());
        PspLoadImageNotifyRoutine += sizeof(PspLoadImageNotifyRoutine);
    }

    return hResult;
}

/// <summary>
/// This will enumerate all Create Thread Routine callbacks registered on a system. It will show the pointer
/// that it's located at, as well as the function of that callback. 
/// </summary>
/// <returns>S_OK if successful</returns>
HRESULT enumerateCreateThreadRoutines()
{
    std::string SymbolName(MAX_SYMBOL_LENGTH, '\0');
    std::ostringstream ss;

    ULONG64 PspCreateThreadNotifyRoutineNonSystemCount = 0;
    ULONG64 PspCreateThreadNotifyRoutineCount = 0;
    ULONG64 PspCreateThreadNotifyRoutine = 0;

    ULONG64 ThreadCallbacksCount = 0;
    ULONG64 temp = 0;

    //
    // Not quite sure if this particular symbol is needed, but it doesn't hurt to include it
    //
    HRESULT hResult = g_pSymbols->GetOffsetByName("nt!PspCreateThreadNotifyRoutineNonSystemCount", &PspCreateThreadNotifyRoutineNonSystemCount);
    if (FAILED(hResult))
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_ERROR, "[%ws::%d] Failed with status: %d\n", __FUNCTIONW__, __LINE__, hResult);
        return hResult;
    }

    hResult = g_pSymbols->GetOffsetByName("nt!PspCreateThreadNotifyRoutineCount", &PspCreateThreadNotifyRoutineCount);
    if (FAILED(hResult))
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_ERROR, "[%ws::%d] Failed with status: %d\n", __FUNCTIONW__, __LINE__, hResult);
        return hResult;
    }

    hResult = g_pSymbols->GetOffsetByName("nt!PspCreateThreadNotifyRoutine", &PspCreateThreadNotifyRoutine);
    if (FAILED(hResult))
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_ERROR, "[%ws::%d] Failed with status: %d\n", __FUNCTIONW__, __LINE__, hResult);
        return hResult;
    }

    //
    // Get the total count for PspCreateThreadNotifyRoutine
    //
    ReadPointer(PspCreateThreadNotifyRoutineNonSystemCount, &temp);
    ThreadCallbacksCount += temp;

    ReadPointer(PspCreateThreadNotifyRoutineCount, &temp);
    ThreadCallbacksCount += temp;

    g_pDebugControl->Output(DEBUG_OUTPUT_DEBUGGEE, "The following Create Thread Notify routines are registered on the system:\n");
    for (int i = 0; i < ThreadCallbacksCount; i++)
    {
        UINT64 ThreadCallbackPointer = 0;
        UINT64 ThreadCallbackAddress = 0;
        ULONG64 Displacement = 0;

        ReadPointer(PspCreateThreadNotifyRoutine, &ThreadCallbackPointer);
        ThreadCallbackPointer &= ~15;
        ThreadCallbackPointer += sizeof(ThreadCallbackPointer);

        ReadPointer(ThreadCallbackPointer, &ThreadCallbackAddress);

        hResult = g_pSymbols->GetNameByOffset(ThreadCallbackAddress, &SymbolName[0], static_cast<ULONG>(SymbolName.size()), NULL, &Displacement);
        if (FAILED(hResult))
        {
            return hResult;
        }

        if (Displacement != 0)
        {
            ss << std::hex << "+" << Displacement;

            SymbolName.insert(strlen(SymbolName.c_str()), ss.str().c_str());

            ss.str(std::string());
        }

        g_pDebugControl->Output(DEBUG_OUTPUT_DEBUGGEE, "%p: %s\n", ThreadCallbackPointer, SymbolName.c_str());
        PspCreateThreadNotifyRoutine += sizeof(PspCreateThreadNotifyRoutine);
    }

    return hResult;
}

/// <summary>
/// This will enumerate all Create Process Routine callbacks registered on a system. It will show the pointer
/// that it's located at, as well as the function of that callback. 
/// </summary>
/// <returns>S_OK if successful</returns>
HRESULT enumerateCreateProcessRoutines()
{
    std::string SymbolName(MAX_SYMBOL_LENGTH, '\0');
    std::ostringstream ss;

    ULONG64 PspCreateProcessNotifyRoutineExCount = 0;
    ULONG64 PspCreateProcessNotifyRoutineCount = 0;
    ULONG64 PspCreateProcessNotifyRoutine = 0;

    ULONG64 ProcessCallbacksCount = 0;
    ULONG64 temp = 0;

    HRESULT hResult = g_pSymbols->GetOffsetByName("nt!PspCreateProcessNotifyRoutineExCount", &PspCreateProcessNotifyRoutineExCount);
    if (FAILED(hResult))
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_ERROR, "[%ws::%d] Failed with status: %d\n", __FUNCTIONW__, __LINE__, hResult);
        return hResult;
    }

    hResult = g_pSymbols->GetOffsetByName("nt!PspCreateProcessNotifyRoutineCount", &PspCreateProcessNotifyRoutineCount);
    if (FAILED(hResult))
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_ERROR, "[%ws::%d] Failed with status: %d\n", __FUNCTIONW__, __LINE__, hResult);
        return hResult;
    }

    hResult = g_pSymbols->GetOffsetByName("nt!PspCreateProcessNotifyRoutine", &PspCreateProcessNotifyRoutine);
    if (FAILED(hResult))
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_ERROR, "[%ws::%d] Failed with status: %d\n", __FUNCTIONW__, __LINE__, hResult);
        return hResult;
    }

    //
    // Get the total count for both PspCreateProcessNotifyRoutine/Ex
    //
    ReadPointer(PspCreateProcessNotifyRoutineExCount, &temp);
    ProcessCallbacksCount += temp;

    ReadPointer(PspCreateProcessNotifyRoutineCount, &temp);
    ProcessCallbacksCount += temp;

    g_pDebugControl->Output(DEBUG_OUTPUT_DEBUGGEE, "The following Create Process Notify routines are registered on the system:\n");
    for (int i = 0; i < ProcessCallbacksCount; i++)
    {
        UINT64 ProcessCallbackPointer = 0;
        UINT64 ProcessCallbackAddress = 0;
        ULONG64 Displacement = 0;

        ReadPointer(PspCreateProcessNotifyRoutine, &ProcessCallbackPointer);
        ProcessCallbackPointer &= ~15;
        ProcessCallbackPointer += sizeof(ProcessCallbackPointer);

        ReadPointer(ProcessCallbackPointer, &ProcessCallbackAddress);

        hResult = g_pSymbols->GetNameByOffset(ProcessCallbackAddress, &SymbolName[0], static_cast<ULONG>(SymbolName.size()), NULL, &Displacement);
        if (FAILED(hResult))
        {
            return hResult;
        }

        if (Displacement != 0)
        {
            ss << std::hex << "+" << Displacement;

            SymbolName.insert(strlen(SymbolName.c_str()), ss.str().c_str());

            ss.str(std::string());
        }

        g_pDebugControl->Output(DEBUG_OUTPUT_DEBUGGEE, "%p: %s\n", ProcessCallbackPointer, SymbolName.c_str());
        PspCreateProcessNotifyRoutine += sizeof(PspCreateProcessNotifyRoutine);
    }

    return hResult;
}

void cleanUpAndExit()
{
    if (g_pSymbols != nullptr)
    {
        g_pSymbols->Release();
    }

    if (g_pDebugControl != nullptr)
    {
        g_pDebugControl->Release();
    }

    if (g_pDebugClient != nullptr)
    {
        g_pDebugClient->Release();
    }
}

CALLBACK_API HRESULT callbacks(PIDebugClient, PCSTR args)
{
    if (*args == NULL)
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_DEBUGGEE, "Please specify a type of callback or check out the help command.\n");
        return S_FALSE;
    }

    if (strcmp(args, "process") == 0)
    {
        enumerateCreateProcessRoutines();
    }
    else if (strcmp(args, "thread") == 0)
    {
        enumerateCreateThreadRoutines();
    }
    else if (strcmp(args, "image") == 0)
    {
        enumerateLoadImageRoutines();
    }
    else if (strcmp(args, "all") == 0)
    {
        enumerateCreateProcessRoutines();
        g_pDebugControl->Output(DEBUG_OUTPUT_DEBUGGEE, "\n");

        enumerateCreateThreadRoutines();
        g_pDebugControl->Output(DEBUG_OUTPUT_DEBUGGEE, "\n");

        enumerateLoadImageRoutines();
    }
    else if (strcmp(args, "help") == 0)
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_DEBUGGEE,
            "Provide a callback type you are interested in.\n"
            "The different types are: process, thread, image, or all\n"
        );
    }
    else
    {
        g_pDebugControl->Output(DEBUG_OUTPUT_DEBUGGEE, "Unable to recognize the callback argument: %s.\n", args);
        return S_FALSE;
    }

    return S_OK;
}


/// EOF