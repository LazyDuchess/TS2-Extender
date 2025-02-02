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

typedef unsigned int(__thiscall* RANDOMUINT32UNIFORM)(TS2::cRZRandom*);

static RANDOMUINT32UNIFORM fpRandomUint32Uniform = NULL;

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

	return true;
}