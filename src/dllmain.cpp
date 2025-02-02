// dllmain.cpp : Defines the entry point for the DLL application.
#include "Core.h"
#include "Logging.h"
#include <iostream>
#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

EXTERN_DLL_EXPORT BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        if (!Core::Create()) {
            Log("Failed to initialize Core!\n");
            return TRUE;
        }
        Log("Core initialized.\n");
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        Core::GetInstance()->Shutdown();
        break;
    }
    return TRUE;
}