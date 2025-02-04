#include "framework.h"
#include <iostream>
#include "Logging.h"
#include "Core.h"
#include "config.h"
#include "PatchVersion.h"
#include "ts2/cRZRandom.h"
#include <chrono>
#include "Addresses.h"
#include "MinHook.h"
#include "scan.h"

typedef unsigned int(__thiscall* RANDOMUINT32UNIFORM)(TS2::cRZRandom*);
typedef LPWSTR(WINAPI* GETCOMMANDLINEW)();
typedef BOOL(WINAPI* SETWINDOWPOS)(HWND, HWND, int, int, int, int, UINT);
typedef UINT(__thiscall* LUA5OPEN)(void*,UINT);

static RANDOMUINT32UNIFORM fpRandomUint32Uniform = NULL;
static GETCOMMANDLINEW fpGetCommandLineW = NULL;
static SETWINDOWPOS fpSetWindowPos = NULL;
static LUA5OPEN fpLua5Open = NULL;
static char placeholderMoviePath[] = "";
static char retOverride[] = { 0xC3 };

static UINT __fastcall DetourLua5Open(void* me, void* _, UINT flags) {
	flags |= 0x2;
	flags |= 0x4;
	flags |= 0x8;
	flags |= 0x10;
	flags |= 0x20;
	Log("Initializing Lua with Flags %i\n", flags);
	return fpLua5Open(me, flags);
}

static bool IsGameWindowTitle(std::wstring windowName) {
	return windowName.find(L"Sims") != std::wstring::npos;
}

static bool IsGameWindow(HWND hWnd) {
	wchar_t windowName[256] = { 0 };
	int length = GetWindowTextW(hWnd, windowName, sizeof(windowName) / sizeof(windowName[0]));

	if (length > 0) {
		std::wstring windowStr(windowName);
		return IsGameWindowTitle(windowStr);
	}
	return false;
}

static BOOL WINAPI DetourSetWindowPos(HWND hWnd, HWND hWndInsertAfter, int X, int Y, int cx, int cy, UINT uFlags) {
	if (Config::Borderless && IsGameWindow(hWnd)) {
		RECT desktopRect;
		GetWindowRect(GetDesktopWindow(), &desktopRect);
		SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE | WS_MINIMIZEBOX | WS_SYSMENU);
		SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_APPWINDOW);
		return fpSetWindowPos(hWnd, HWND_TOP, 
			desktopRect.left, desktopRect.top, desktopRect.right - desktopRect.left, desktopRect.bottom - desktopRect.top,
			SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	}
	return fpSetWindowPos(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);
}

static LPWSTR WINAPI DetourGetCommandLineW() {
	LPWSTR comm = fpGetCommandLineW();
	std::wstring newArgs = comm;
	if (Config::Borderless || Config::Windowed)
	{
		newArgs += L" -w";
	}
	return const_cast<wchar_t*>(newArgs.c_str());
}

static unsigned int __fastcall DetourRandomUint32Uniform(TS2::cRZRandom* me, void* _) {
	if (me->Seed == 0) {
		me->Seed = static_cast<unsigned int>(std::chrono::steady_clock::now().time_since_epoch().count());
	}
	return fpRandomUint32Uniform(me);
}

Core* Core::_instance = nullptr;

bool Core::Create() {
	_instance = new Core();
	return _instance->Initialize();
}

bool Core::Initialize() {
	Config::Load();

	if (Config::Console) {
		AllocConsole();
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
	}

	Log("TS2 Extender %s\n", Version);

	Log("Core initializing\n");

	if (!Addresses::Initialize()) return false;

	// Initialize MinHook.
	if (MH_Initialize() != MH_OK)
		return false;

	if (Config::FixRNG) {
		if (MH_CreateHook(Addresses::RandomUint32Uniform, &DetourRandomUint32Uniform,
			reinterpret_cast<LPVOID*>(&fpRandomUint32Uniform)) != MH_OK)
		{
			return false;
		}
		if (MH_EnableHook(Addresses::RandomUint32Uniform) != MH_OK)
		{
			return false;
		}
	}

	if (Config::SkipIntro) {
		void* addrToMovie = placeholderMoviePath;
		WriteToMemory((DWORD)Addresses::EALogoPush, &addrToMovie, 4);
		WriteToMemory((DWORD)Addresses::IntroPush, &addrToMovie, 4);
	}

	if (MH_CreateHook(&GetCommandLineW, &DetourGetCommandLineW,
		reinterpret_cast<LPVOID*>(&fpGetCommandLineW)) != MH_OK)
	{
		return false;
	}
	if (MH_EnableHook(&GetCommandLineW) != MH_OK)
	{
		return false;
	}

	if (MH_CreateHook(&SetWindowPos, &DetourSetWindowPos,
		reinterpret_cast<LPVOID*>(&fpSetWindowPos)) != MH_OK)
	{
		return false;
	}
	if (MH_EnableHook(&SetWindowPos) != MH_OK)
	{
		return false;
	}

	if (Config::ExtendedLua) {
		WriteToMemory((DWORD)Addresses::LuaUnregister, retOverride, 1);
		Nop((BYTE*)Addresses::LuaPrintStub, 16);
		if (MH_CreateHook(Addresses::GZLua5Open, &DetourLua5Open,
			reinterpret_cast<LPVOID*>(&fpLua5Open)) != MH_OK)
		{
			return false;
		}
		if (MH_EnableHook(Addresses::GZLua5Open) != MH_OK)
		{
			return false;
		}
	}

	return true;
}