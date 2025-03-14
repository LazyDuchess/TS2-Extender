// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include "Core.h"
#include "Logging.h"
#include <iostream>

static bool IsGame() {
    char path[MAX_PATH];
    if (GetModuleFileName(NULL, path, MAX_PATH)) {
        std::string filename(path);
        size_t pos = filename.find_last_of("\\");
        if (pos != std::string::npos)
        {
            filename = filename.substr(pos + 1);
        }
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
#if FORCE_CONSOLE
        AllocConsole();
        freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
        freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
        freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
#endif
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