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
#include "LuaExtensions.h"

typedef unsigned int(__thiscall* RANDOMUINT32UNIFORM)(TS2::cRZRandom*);
typedef UINT(__thiscall* LUA5OPEN)(void*,UINT);

static RANDOMUINT32UNIFORM fpRandomUint32Uniform = NULL;
static LUA5OPEN fpLua5Open = NULL;
static char placeholderMoviePath[] = "";
static char retOverride[] = { 0xC3 };

static UINT __fastcall DetourLua5Open(void* me, void* _, UINT flags) {
	flags |= 0x2;
	flags |= 0x4;
	flags |= 0x8;
	flags |= 0x10;
	flags |= 0x20;
	return fpLua5Open(me, flags);
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

#if !FORCE_CONSOLE
	if (Config::Console) {
		AllocConsole();
		freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
		freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
		freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
	}
#endif

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
		if (!LuaExtensions::Initialize()) return false;
	}

	return true;
}