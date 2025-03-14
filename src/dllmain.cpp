// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include "Core.h"
#include "Logging.h"
#include <iostream>

static bool IsGame() {
    char path[MAX_PATH];
    if (GetModuleFileName(NULL, path, MAX_PATH)) {
        std::string filename(path);
        if (filename.find("crashpad") != std::string::npos) return false;
        return true;
    }
    return false;
}

BOOL WINAPI DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        if (!IsGame()) return TRUE;
        if (!Core::Create()) {
            Log("Failed to initialize Core!\n");
            return TRUE;
        }
        Log("Core initialized.\n");
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}