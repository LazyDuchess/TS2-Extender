// dllmain.cpp : Defines the entry point for the DLL application.
#include "framework.h"
#include "Core.h"
#include "Logging.h"
#include "Utils.h"
#include <iostream>
#include "SplashWindow.h"

static bool IsGame() {
#if TS2_UC
    return true;
#else
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
#endif
}

static void CacheCoreDirectory(HMODULE handle) {
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(handle, path, MAX_PATH);
    std::wstring finalPath = path;
    size_t pos = finalPath.find_last_of(L"\\/");
    Core::DllPath = WCharToString(finalPath.substr(0, pos).c_str());
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
        CacheCoreDirectory(hModule);
        SplashWindow::gModule = hModule;
        if (!Core::Create()) {
            Log("Failed to initialize Core!\n");
            SplashWindow::SignalClose();
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