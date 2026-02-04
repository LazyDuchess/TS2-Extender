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
#include <windows.h>
#include <Shlobj.h>
#include "ts2/cTSString.h"
#include "ts2/cRZString.h"

typedef unsigned int(__thiscall* RANDOMUINT32UNIFORM)(TS2::cRZRandom*);
typedef UINT(__thiscall* LUA5OPEN)(void*,UINT);

typedef unsigned int(__thiscall* DIALOGONATTACH)(void* me, void* unk1, int unk2);
typedef unsigned int(__thiscall* CLOTHINGDIALOGONCANCEL)(void* me);

typedef bool(__thiscall* TSSTRINGLOAD)(void* me);

typedef bool(__cdecl* LOADUISCRIPT)(uint32_t instance, void* unk1, void* unk2, void* unk3, bool resolution);
typedef cRZString*(__cdecl* MAKEMONEYSTRING)(int money);

static MAKEMONEYSTRING fpMakeMoneyString = NULL;
static LOADUISCRIPT fpLoadUiScript = NULL;
static TSSTRINGLOAD fpTSStringLoad = NULL;
static RANDOMUINT32UNIFORM fpRandomUint32Uniform = NULL;
static LUA5OPEN fpLua5Open = NULL;
static char placeholderMoviePath[] = "";
static char retOverride[] = { 0xC3 };

static DIALOGONATTACH fpClothingDialogOnAttach = NULL;
static DIALOGONATTACH fpDressEmployeeDialogOnAttach = NULL;
static CLOTHINGDIALOGONCANCEL fpClothingDialogOnCancel = NULL;

static bool CancelNextClothingDialog = false;
static void* ClothingDialogHook1Return;
static void* ClothingDialogHook2Return;

// Simply a PUSH 01 with 2 NOPS. Tells function to always keep separates visible in UI.
static char separatesPatch[] = { 0x6A, 0x01, 0x90, 0x90 };
// PUSH 01 with 1 NOP. Tells buy and try on to keep separates visibles in UI.
static char separatesBuyPatch[] = { 0x6A, 0x01, 0x90 };

static void __declspec(naked) ClothingDialogHook1() {
	__asm {
		cmp[esi + 0xE8], 0x00000000
		je goBack
		mov[esi + 0x000000E8], ecx
		goBack :
			jmp [ClothingDialogHook1Return]
	}
}

static void __declspec(naked) ClothingDialogHook2() {
	__asm {
		cmp[esi+0xE8], 0x00000000
		je goBack
		or dword ptr [esi+0xE8],0x01
		goBack:
			jmp [ClothingDialogHook2Return]
	}
}

static cRZString* __cdecl DetourMakeMoneyString(int money) {
	cRZString* result = fpMakeMoneyString(money);
	if (Core::_instance->m_MakeMoneyStringLuaState == nullptr)
		return result;
	lua_State* luaState = Core::_instance->m_MakeMoneyStringLuaState;
	int luaCall = Core::_instance->m_MakeMoneyStringLuaCall;
	lua_rawgeti(luaState, LUA_REGISTRYINDEX, luaCall);
	lua_pushnumber(luaState, static_cast<double>(money));
	if (lua_pcall(luaState, 1, 1, 0) != 0) {
		Log("Lua MakeMoneyString Override failed: %s\n", lua_tostring(luaState, -1));
		lua_pop(luaState, 1);
	}
	else
	{
		const char* str = lua_tostring(luaState, -1);
		result->FromChar(str);
		lua_pop(luaState, 1);
	}
	return result;
}

static bool __cdecl DetourLoadUIScript(uint32_t instance, void* unk1, void* unk2, void* unk3, bool resolution) {
	auto it = Core::_instance->m_UIOverrides.find(instance);
	if (it != Core::_instance->m_UIOverrides.end()) {
		instance = it->second;
	}
	return fpLoadUiScript(instance, unk1, unk2, unk3, resolution);
}

static bool __fastcall DetourTSStringLoad(cTSString* me, void* _) {
	auto it = Core::_instance->m_StringOverrides.find(StringId(me->GetIndex(), me->GetInstance(), me->GetGroup()));
	if (it != Core::_instance->m_StringOverrides.end()) {
		me->SetIndex(it->second.Index());
		me->SetInstance(it->second.Instance());
		me->SetGroup(it->second.Group());
	}
	return fpTSStringLoad(me);
}

static unsigned int __fastcall DetourDressEmployeeDialogOnAttach(void* me, void* _, void* unk1, int unk2) {
	CancelNextClothingDialog = true;
	return fpDressEmployeeDialogOnAttach(me, unk1, unk2);
}

static unsigned int __fastcall DetourClothingDialogOnAttach(void* me, void* _, void* unk1, int unk2) {
	Log("Created Clothing Dialog: %p, Window: %p\n", me, unk1);
	if (CancelNextClothingDialog) {
		CancelNextClothingDialog = false;
		int res = fpClothingDialogOnAttach(me, unk1, unk2);
		((void(__thiscall*)(void*))Addresses::ClothingDialogOnCancel)(me);
		((char*)unk1)[0xE8] = 0x00;
		((char*)unk1)[0xE9] = 0x00;
		((char*)unk1)[0xEA] = 0x00;
		((char*)unk1)[0xEB] = 0x00;
		return 0;
	}
	return fpClothingDialogOnAttach(me, unk1, unk2);
}

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

void Core::CacheUserData() {
	HKEY nameKey;

	LSTATUS keyStatus = RegOpenKeyExW(
		HKEY_CURRENT_USER,
		L"SOFTWARE\\Electronic Arts\\The Sims 2 Ultimate Collection 25",
		0,
		KEY_READ | KEY_WOW64_32KEY,
		&nameKey
	);

	if (keyStatus != ERROR_SUCCESS) return;

	DWORD finalSize = 0;
	const wchar_t keyName[] = L"displayname";

	LSTATUS valueStatus = RegGetValueW(
		nameKey,
		NULL,
		keyName,
		RRF_RT_REG_SZ,
		NULL,
		NULL,
		&finalSize
	);

	if (valueStatus != ERROR_SUCCESS) return;

	m_GameDisplayName.resize(finalSize / sizeof(wchar_t));

	RegGetValueW(
		nameKey,
		NULL,
		keyName,
		RRF_RT_REG_SZ,
		NULL,
		m_GameDisplayName.data(),
		&finalSize
	);

	PWSTR path_pwstr;
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, NULL, &path_pwstr);

	if (SUCCEEDED(hr)) {
		m_UserDataPath = std::wstring(path_pwstr) + L"\\EA Games\\" + m_GameDisplayName;
		CoTaskMemFree(path_pwstr);
	}
}

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

	Core::_instance->CacheUserData();

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

	if (Config::FixOFBUniform) {
		if (MH_CreateHook(Addresses::ClothingDialogOnAttach, &DetourClothingDialogOnAttach,
			reinterpret_cast<LPVOID*>(&fpClothingDialogOnAttach)) != MH_OK)
		{
			return false;
		}
		if (MH_EnableHook(Addresses::ClothingDialogOnAttach) != MH_OK)
		{
			return false;
		}

		if (MH_CreateHook(Addresses::DressEmployeeDialogOnAttach, &DetourDressEmployeeDialogOnAttach,
			reinterpret_cast<LPVOID*>(&fpDressEmployeeDialogOnAttach)) != MH_OK)
		{
			return false;
		}
		if (MH_EnableHook(Addresses::DressEmployeeDialogOnAttach) != MH_OK)
		{
			return false;
		}

		Nop((BYTE*)Addresses::ClothingDialogSetState, 10);

		ClothingDialogHook1Return = (void*)((DWORD)Addresses::ClothingDialogHack1 + 6);
		ClothingDialogHook2Return = (void*)((DWORD)Addresses::ClothingDialogHack2 + 7);
		MakeJMP((BYTE*)Addresses::ClothingDialogHack1, (DWORD)ClothingDialogHook1, 6);
		MakeJMP((BYTE*)Addresses::ClothingDialogHack2, (DWORD)ClothingDialogHook2, 7);
	}

	if (Config::Separates4All) {
		WriteToMemory((DWORD)Addresses::CalculateOutfitPartVisibility, &separatesPatch, 4);

		WriteToMemory((DWORD)Addresses::CalculateBuyPartVisibility, &separatesBuyPatch, 3);
		WriteToMemory((DWORD)Addresses::CalculateBuyPartVisibility + 0xE, &separatesBuyPatch, 3);
		WriteToMemory((DWORD)Addresses::CalculateBuyPartVisibility + 0xE + 0xE, &separatesBuyPatch, 3);

		WriteToMemory((DWORD)Addresses::CalculateTryOnPartVisibility, &separatesBuyPatch, 3);
		WriteToMemory((DWORD)Addresses::CalculateTryOnPartVisibility + 0xE, &separatesBuyPatch, 3);
		WriteToMemory((DWORD)Addresses::CalculateTryOnPartVisibility + 0xE + 0xE, &separatesBuyPatch, 3);
	}

	
	if (MH_CreateHook(Addresses::TSStringLoad, &DetourTSStringLoad,
		reinterpret_cast<LPVOID*>(&fpTSStringLoad)) != MH_OK)
	{
		return false;
	}
	if (MH_EnableHook(Addresses::TSStringLoad) != MH_OK)
	{
		return false;
	}

	if (MH_CreateHook(Addresses::LoadUIScript, &DetourLoadUIScript,
		reinterpret_cast<LPVOID*>(&fpLoadUiScript)) != MH_OK)
	{
		return false;
	}
	if (MH_EnableHook(Addresses::LoadUIScript) != MH_OK)
	{
		return false;
	}

	if (MH_CreateHook(Addresses::UIMakeMoneyString, &DetourMakeMoneyString,
		reinterpret_cast<LPVOID*>(&fpMakeMoneyString)) != MH_OK)
	{
		return false;
	}
	if (MH_EnableHook(Addresses::UIMakeMoneyString) != MH_OK)
	{
		return false;
	}

	return true;
}