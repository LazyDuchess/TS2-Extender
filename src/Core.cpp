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
#include "ts2/cTSGlobals.h"
#include "ts2/cEdithObjectTestSim.h"
#include "ts2/cTSPerson.h"
#include "ts2/cTSInteraction.h"
#include <cstring>
#include <string>
#include "ts2/cGZMessage.h"
#include "Utils.h"

typedef unsigned int(__thiscall* RANDOMUINT32UNIFORM)(TS2::cRZRandom*);
typedef UINT(__thiscall* LUA5OPEN)(void*, UINT);

typedef unsigned int(__thiscall* DIALOGONATTACH)(void* me, void* unk1, int unk2);
typedef unsigned int(__thiscall* CLOTHINGDIALOGONCANCEL)(void* me);

typedef bool(__thiscall* TSSTRINGLOAD)(void* me);

typedef bool(__cdecl* LOADUISCRIPT)(uint32_t instance, void* unk1, void* unk2, void* unk3, bool resolution);
typedef cRZString* (__cdecl* MAKEMONEYSTRING)(int money);
typedef void(__thiscall* APPENDINTERACTIONSFORMENU)(cEdithObjectTestSim* testSim, std::vector<cTSInteraction*>* interactions, bool debug);

typedef int(__thiscall* OVERLAYSONTICK)(void* self, int unk);
typedef int(__thiscall* OVERLAYSDOMESSAGE)(void* self, cGZMessage* msg);
typedef int(__thiscall* OVERLAYSACTIVATE)(void* self);

typedef void(__thiscall* ONCEPERFRAMEUPDATE)(void* self);

typedef int(__thiscall* ADDGAMEVERSION)(void* self);

static ADDGAMEVERSION fpAddGameVersion = NULL;
static ONCEPERFRAMEUPDATE fpOncePerFrameUpdate = NULL;
static OVERLAYSACTIVATE fpOverlaysActivate = NULL;
static OVERLAYSDOMESSAGE fpOverlaysDoMessage = NULL;
static OVERLAYSONTICK fpOverlaysOnTick = NULL;
static APPENDINTERACTIONSFORMENU fpAppendInteractionsForMenu = NULL;
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

static unsigned int modifyVoiceEventObjectId = 0;
static void* ModifyVoiceEventHook1Return;
static void* ModifyVoiceEventHook2Return;

// Store the Object ID for cEMVoxModifier::ModifyEvent, contained in ESI.
static void __declspec(naked) ModifyVoiceEventHook1() {
	__asm {
		mov [modifyVoiceEventObjectId], esi
		mov ecx, eax
		mov edx, [eax]
		call dword ptr [edx+0x68]
		jmp[ModifyVoiceEventHook1Return]
	}
}

static int MakeLuaTableForModifyVoiceEvent(lua_State* luaState, cRZString* str, unsigned int objectId) {
	lua_newtable(luaState);
	int tableId = lua_gettop(luaState);

	char* strptr = str->GetString();
	int underscoreIndex = -1;

	for (int i = std::strlen(strptr) - 1; i >= 0; --i) {
		if (strptr[i] == '_') {
			underscoreIndex = i;
			strptr[i] = '\0';
			break;
		}
	}

	lua_pushstring(luaState, "Vox");
	lua_pushstring(luaState, strptr);
	lua_settable(luaState, -3);

	lua_pushstring(luaState, "Suffix");
	if (underscoreIndex == -1)
		lua_pushstring(luaState, "");
	else
		lua_pushstring(luaState, &strptr[underscoreIndex + 1]);
	lua_settable(luaState, -3);

	lua_pushstring(luaState, "PersonId");
	lua_pushnumber(luaState, static_cast<double>(objectId));
	lua_settable(luaState, -3);

	return tableId;
}

static void __stdcall ModifyVoiceEventInternalCall(cRZString* str) {
	Core* core = Core::_instance;
	int tableId = MakeLuaTableForModifyVoiceEvent(core->m_LuaState, str, modifyVoiceEventObjectId);
	for (auto& cb : core->m_LuaDelegates[(int)Delegates::OnModifyVoiceEvent].m_Callbacks) {
		lua_rawgeti(cb.m_luaState, LUA_REGISTRYINDEX, cb.m_LuaCall);
		lua_pushvalue(cb.m_luaState, tableId);
		if (lua_pcall(cb.m_luaState, 1, 0, 0) != 0) {
			Log("Error calling Lua callback: %s\n", lua_tostring(cb.m_luaState, -1));
			lua_pop(cb.m_luaState, 1);
		}
	}

	lua_pushvalue(core->m_LuaState, tableId);
	lua_pushstring(core->m_LuaState, "Vox");
	lua_gettable(core->m_LuaState, -2);

	std::string finalStr(lua_tostring(core->m_LuaState, -1));

	lua_pop(core->m_LuaState, 1);

	lua_pushvalue(core->m_LuaState, tableId);
	lua_pushstring(core->m_LuaState, "Suffix");
	lua_gettable(core->m_LuaState, -2);

	std::string suffixStr(lua_tostring(core->m_LuaState, -1));

	lua_pop(core->m_LuaState, 1);

	if (!suffixStr.empty()) {
		finalStr += "_";
		finalStr += suffixStr;
	}

	str->FromChar(finalStr.c_str());
}

// At this point, the built cRZString for the voice filename is in EAX.
static void __declspec(naked) ModifyVoiceEventHook2() {
	__asm {
		push eax
		push ebx
		push ecx
		push edx
		push esi
		push edi
		push ebp

		push eax
		call ModifyVoiceEventInternalCall

		pop ebp
		pop edi
		pop esi
		pop edx
		pop ecx
		pop ebx
		pop eax

		mov eax, [edi]
		lea ecx, [ebp - 0x4C]
		jmp[ModifyVoiceEventHook2Return]
	}
}

// Override visibility flags for clothing dialog for employee dress fix.
#if TS2_LC
static void __declspec(naked) ClothingDialogHook1() {
	__asm {
		cmp[esi + 0xE8], 0x00000000
		je goBack
		mov[esi + 0x000000E8], ecx
		goBack :
		jmp[ClothingDialogHook1Return]
	}
}

static void __declspec(naked) ClothingDialogHook2() {
	__asm {
		cmp[esi + 0xE8], 0x00000000
		je goBack
		or dword ptr[esi + 0xE8], 0x01
		goBack:
		jmp[ClothingDialogHook2Return]
	}
}
#else
static void __declspec(naked) ClothingDialogHook1() {
	__asm {
		cmp[esi + 0xD8], 0x00000000
		je goBack
		mov[esi + 0x000000D8], edx
		goBack :
		jmp[ClothingDialogHook1Return]
	}
}

static void __declspec(naked) ClothingDialogHook2() {
	__asm {
		cmp[esi + 0xD8], 0x00000000
		je goBack
		or dword ptr[esi + 0xD8], 0x01
		goBack:
		jmp[ClothingDialogHook2Return]
	}
}
#endif

static void AddCheatInteraction(std::vector<cTSInteraction*>* interactions, cTSPerson* person, cTSObject* object, int interactionType, short flags, const char* name, short instanceId) {
	((void(__cdecl*)(std::vector<cTSInteraction*>*, void*, void*, int, short, const char*, short))Addresses::AddCheatInteraction)(interactions, person, object, interactionType, flags, name, instanceId);
}

static int __cdecl LuaVectorClear(lua_State* luaState) {
	lua_pushstring(luaState, "_handle");
	lua_gettable(luaState, 1);
	std::string handleStr(lua_tostring(luaState, -1));
	std::vector<cTSInteraction*>* vec = (std::vector<cTSInteraction*>*)std::stoi(handleStr, nullptr);
	lua_pop(luaState, 1);
	vec->clear();
	return 0;
}

// vec:AddInteraction(number interactionInstanceId, number interactionObjectId, bool blocking, string interactionName, number param0, number param1, number param2, number param3)
static int __cdecl LuaAddInteraction(lua_State* luaState) {
	Core* core = Core::_instance;
	cTSGlobals* globals = cTSGlobals::GetInstance();
	cEdithObjectModule* objManager = globals->ObjectManager();

	lua_pushstring(luaState, "_handle");
	lua_gettable(luaState, 1);
	std::string handleStr(lua_tostring(luaState, -1));
	std::vector<cTSInteraction*>* vec = (std::vector<cTSInteraction*>*)std::stoi(handleStr, nullptr);
	lua_pop(luaState, 1);

	short instance = static_cast<short>(lua_tonumber(luaState, 2));
	short interactionObjectId = static_cast<short>(lua_tonumber(luaState, 3));
	bool blocking = lua_toboolean(luaState, 4) != 0;
	const char* name = lua_tostring(luaState, 5);

	short param0 = static_cast<short>(lua_tonumber(luaState, 6));
	short param1 = static_cast<short>(lua_tonumber(luaState, 7));
	short param2 = static_cast<short>(lua_tonumber(luaState, 8));
	short param3 = static_cast<short>(lua_tonumber(luaState, 9));

	cTSObject* interactionObject = objManager->GetObjectFromID(interactionObjectId);

	AddCheatInteraction(vec, core->m_CurrentTestSim->GetPerson(), interactionObject, blocking ? -1 : -2, 0x32, name, instance);

	cTSInteraction* newInteraction = vec->back();

	newInteraction->SetTargetObject(core->m_CurrentTestSim->GetObj());
	short stackVars[] = { param0, param1, param2, param3 };
	newInteraction->SetStackVars(stackVars);
	return 0;
}

// vec:Clear()
static int MakeLuaTableForInteractionVector(lua_State* luaState, std::vector<cTSInteraction*>* vec){
	lua_newtable(luaState);
	int tableId = lua_gettop(luaState);

	lua_pushstring(luaState, "_handle");
	lua_pushstring(luaState, std::to_string((DWORD)vec).c_str());
	lua_settable(luaState, -3);

	lua_pushstring(luaState, "Clear");
	lua_pushcclosure(luaState, &LuaVectorClear, 0);
	lua_settable(luaState, -3);

	lua_pushstring(luaState, "AddInteraction");
	lua_pushcclosure(luaState, &LuaAddInteraction, 0);
	lua_settable(luaState, -3);

	return tableId;
}

static int __fastcall DetourAddGameVersion(void* self, void* _) {
	return 0;
}

static bool shouldTickOverlays = false;

static void __fastcall DetourOncePerFrameUpdate(void* self, void* _) {
	fpOncePerFrameUpdate(self);
	shouldTickOverlays = false;
	Core* core = Core::_instance;
	for (auto& cb : core->m_LuaDelegates[(int)Delegates::OnFrameUpdate].m_Callbacks) {
		lua_rawgeti(cb.m_luaState, LUA_REGISTRYINDEX, cb.m_LuaCall);
		if (lua_pcall(cb.m_luaState, 0, 0, 0) != 0) {
			Log("Error calling Lua callback: %s\n", lua_tostring(cb.m_luaState, -1));
			lua_pop(cb.m_luaState, 1);
		}
	}
	LuaExtensions::FrameUpdate();
}

static int __fastcall DetourOverlaysActivate(void* self, void* _) {
	shouldTickOverlays = true;
	return fpOverlaysActivate(self);
}

static int __fastcall DetourOverlaysDoMessage(void* self, void* _, cGZMessage* msg) {
	if ((msg->MessageID == 0x3) && (msg->Unknown == 0x287259f6 || msg->Unknown == 0x28759f7 || msg->Unknown == 0x28759f8))
		shouldTickOverlays = true;
	return fpOverlaysDoMessage(self, msg);
}

static void* UITabChangeHookReturn;

static void __declspec(naked) UITabChangeHook() {
	__asm {
		mov[shouldTickOverlays], 0x1
		mov eax,[edi+0x08]
		mov [ebp+0x0C], eax
		jmp [UITabChangeHookReturn]
	}
}

static void* UIMirrorTabChangeHookReturn;

static void __declspec(naked) UIMirrorTabChangeHook() {
	__asm {
		mov[shouldTickOverlays], 0x1
		call dword ptr [edx+0x00000154]
		jmp[UIMirrorTabChangeHookReturn]
	}
}

static int __fastcall DetourOverlaysOnTick(void* self, void* _, int unk) {
	if (shouldTickOverlays) {
		return fpOverlaysOnTick(self, unk);
	}
	return 1;
}

// Callback(vec Interactions, number SimId, number ObjectId, bool clicked, bool debug)
static void __fastcall DetourAppendInteractionsForMenu(cEdithObjectTestSim* testSim, void* _, std::vector<cTSInteraction*>* interactions, bool clicked) {
	fpAppendInteractionsForMenu(testSim, interactions, clicked);
	Core* core = Core::_instance;
	core->m_CurrentTestSim = testSim;
	cTSGlobals* globals = cTSGlobals::GetInstance();
	bool debug = testSim->GetObj()->GetMiscFlag(0x2000) && globals->TestingCheatsEnabled();
	int tableId = MakeLuaTableForInteractionVector(core->m_LuaState, interactions);
	for (auto& cb : core->m_LuaDelegates[(int)Delegates::OnBuildPieMenu].m_Callbacks) {
		lua_rawgeti(cb.m_luaState, LUA_REGISTRYINDEX, cb.m_LuaCall);
		lua_pushvalue(cb.m_luaState, tableId);
		lua_pushnumber(cb.m_luaState, testSim->GetPerson()->AsEdithObject()->GetID());
		lua_pushnumber(cb.m_luaState, testSim->GetObj()->GetID());
		lua_pushboolean(cb.m_luaState, clicked ? 1 : 0);
		lua_pushboolean(cb.m_luaState, debug ? 1 : 0);
		if (lua_pcall(cb.m_luaState, 5, 0, 0) != 0) {
			Log("Error calling Lua callback: %s\n", lua_tostring(cb.m_luaState, -1));
			lua_pop(cb.m_luaState, 1);
		}
	}
	core->m_CurrentTestSim = nullptr;
}

static cRZString* __cdecl DetourMakeMoneyString(int money) {
	cRZString* result = fpMakeMoneyString(money);
	if (Core::_instance->m_MakeMoneyStringLuaCall == LUA_NOREF)
		return result;
	lua_State* luaState = Core::_instance->m_LuaState;
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
	if (Core::_instance->m_LoadUIScriptDebug)
		Log("Loading UIScript %X\n", instance);
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
#if TS2_LC
		((char*)unk1)[0xE8] = 0x00;
		((char*)unk1)[0xE9] = 0x00;
		((char*)unk1)[0xEA] = 0x00;
		((char*)unk1)[0xEB] = 0x00;
#else
		((char*)unk1)[0xD8] = 0x00;
		((char*)unk1)[0xD9] = 0x00;
		((char*)unk1)[0xDA] = 0x00;
		((char*)unk1)[0xDB] = 0x00;
#endif
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

void Core::DoDefaultUserData() {
	m_GameDisplayName = L"The Sims 2";
	PWSTR path_pwstr;
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, KF_FLAG_DEFAULT, NULL, &path_pwstr);
	if (SUCCEEDED(hr)) {
		m_UserDataPath = std::wstring(path_pwstr) + L"\\EA Games\\" + m_GameDisplayName;
		CoTaskMemFree(path_pwstr);
	}
}

bool Core::CacheUserData() {
	HKEY nameKey;

#if TS2_LC
	LSTATUS keyStatus = RegOpenKeyExW(
		HKEY_CURRENT_USER,
		L"SOFTWARE\\Electronic Arts\\The Sims 2 Ultimate Collection 25",
		0,
		KEY_READ | KEY_WOW64_32KEY,
		&nameKey
	);
#else
	LSTATUS keyStatus = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SOFTWARE\\EA GAMES\\The Sims 2",
		0,
		KEY_READ | KEY_WOW64_32KEY,
		&nameKey
	);
#endif

	if (keyStatus != ERROR_SUCCESS) return false;

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

	if (valueStatus != ERROR_SUCCESS) return false;

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
		Log("Found registry for game user folder: %s\n", WCharToString(m_UserDataPath.c_str()).c_str());
		return true;
	}
	return false;
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

	if (!Core::_instance->CacheUserData())
	{
		Core::_instance->DoDefaultUserData();
		Log("Failed to find registry for game user folder, using default Documents/EA Games/The Sims 2 folder.");
	}

	// Initialize MinHook.
	if (MH_Initialize() != MH_OK)
	{
		Log("Failed to initialize MinHook\n");
		return false;
	}

	if (Config::FixRNG && ADDRESS_VALID(Addresses::RandomUint32Uniform)) {
		if (MH_CreateHook(Addresses::RandomUint32Uniform, &DetourRandomUint32Uniform,
			reinterpret_cast<LPVOID*>(&fpRandomUint32Uniform)) != MH_OK)
		{
			Log("FixRNG Patch Failed!\n");
			return false;
		}
		if (MH_EnableHook(Addresses::RandomUint32Uniform) != MH_OK)
		{
			Log("FixRNG Patch Failed!\n");
			return false;
		}
	}

	if (Config::SkipIntro && ADDRESS_VALID(Addresses::EALogoPush) && ADDRESS_VALID(Addresses::IntroPush)) {
		void* addrToMovie = placeholderMoviePath;
		WriteToMemory((DWORD)Addresses::EALogoPush, &addrToMovie, 4);
		WriteToMemory((DWORD)Addresses::IntroPush, &addrToMovie, 4);
	}

	// TODO: Improve checks for those that rely on multiple addresses

	bool luaValid = ADDRESS_VALID(Addresses::GZLua5Open) && ADDRESS_VALID(Addresses::LuaUnregister) && ADDRESS_VALID(Addresses::RegisterLuaCommands) && ADDRESS_VALID(Addresses::RegisterTSSGCheats);

	if (Config::ExtendedLua) {
		if (ADDRESS_VALID(Addresses::LuaUnregister))
			WriteToMemory((DWORD)Addresses::LuaUnregister, retOverride, 1);

		if (ADDRESS_VALID(Addresses::LuaPrintStub)) {
#if TS2_LC
			Nop((BYTE*)Addresses::LuaPrintStub, 16);
#else
			// Nop 2 arg pushes and the call later.
			Nop((BYTE*)Addresses::LuaPrintStub, 10);
			Nop((BYTE*)((DWORD)Addresses::LuaPrintStub + 14), 6);
#endif
		}
		if (ADDRESS_VALID(Addresses::GZLua5Open)) {
			if (MH_CreateHook(Addresses::GZLua5Open, &DetourLua5Open,
				reinterpret_cast<LPVOID*>(&fpLua5Open)) != MH_OK)
			{
				Log("ExtendedLua Patch Failed!\n");
				return false;
			}
			if (MH_EnableHook(Addresses::GZLua5Open) != MH_OK)
			{
				Log("ExtendedLua Patch Failed!\n");
				return false;
			}
		}
		if (luaValid) {
			if (!LuaExtensions::Initialize()) return false;
		}
	}

	if (Config::FixPinkFlashing && ADDRESS_VALID(Addresses::LAAPointerCheck)) {
		Nop((BYTE*)Addresses::LAAPointerCheck, 4);
	}

	if (Config::FixOFBUniform && ADDRESS_VALID(Addresses::ClothingDialogOnAttach)) {
		if (MH_CreateHook(Addresses::ClothingDialogOnAttach, &DetourClothingDialogOnAttach,
			reinterpret_cast<LPVOID*>(&fpClothingDialogOnAttach)) != MH_OK)
		{
			Log("FixOFBUniform Patch Failed!\n");
			return false;
		}
		if (MH_EnableHook(Addresses::ClothingDialogOnAttach) != MH_OK)
		{
			Log("FixOFBUniform Patch Failed!\n");
			return false;
		}

		if (MH_CreateHook(Addresses::DressEmployeeDialogOnAttach, &DetourDressEmployeeDialogOnAttach,
			reinterpret_cast<LPVOID*>(&fpDressEmployeeDialogOnAttach)) != MH_OK)
		{
			Log("FixOFBUniform Patch Failed!\n");
			return false;
		}
		if (MH_EnableHook(Addresses::DressEmployeeDialogOnAttach) != MH_OK)
		{
			Log("FixOFBUniform Patch Failed!\n");
			return false;
		}

#if TS2_LC
		Nop((BYTE*)Addresses::ClothingDialogSetState, 10);
#else
		Nop((BYTE*)Addresses::ClothingDialogSetState, 14);
#endif

		ClothingDialogHook1Return = (void*)((DWORD)Addresses::ClothingDialogHack1 + 6);
		ClothingDialogHook2Return = (void*)((DWORD)Addresses::ClothingDialogHack2 + 7);
		MakeJMP((BYTE*)Addresses::ClothingDialogHack1, (DWORD)ClothingDialogHook1, 6);
		MakeJMP((BYTE*)Addresses::ClothingDialogHack2, (DWORD)ClothingDialogHook2, 7);
	}

	if (Config::FixMakeupLag && ADDRESS_VALID(Addresses::cTSUICASComponentOverlaysOnTick)) {
		if (MH_CreateHook(Addresses::cTSUICASComponentOverlaysOnTick, &DetourOverlaysOnTick,
			reinterpret_cast<LPVOID*>(&fpOverlaysOnTick)) != MH_OK)
		{
			Log("FixMakeupLag Patch Failed!\n");
			return false;
		}
		if (MH_EnableHook(Addresses::cTSUICASComponentOverlaysOnTick) != MH_OK)
		{
			Log("FixMakeupLag Patch Failed!\n");
			return false;
		}

		if (MH_CreateHook(Addresses::cTSUICASComponentOverlaysDoMessage, &DetourOverlaysDoMessage,
			reinterpret_cast<LPVOID*>(&fpOverlaysDoMessage)) != MH_OK)
		{
			Log("FixMakeupLag Patch Failed!\n");
			return false;
		}
		if (MH_EnableHook(Addresses::cTSUICASComponentOverlaysDoMessage) != MH_OK)
		{
			Log("FixMakeupLag Patch Failed!\n");
			return false;
		}

		if (MH_CreateHook(Addresses::cTSUICASComponentOverlaysActivate, &DetourOverlaysActivate,
			reinterpret_cast<LPVOID*>(&fpOverlaysActivate)) != MH_OK)
		{
			Log("FixMakeupLag Patch Failed!\n");
			return false;
		}
		if (MH_EnableHook(Addresses::cTSUICASComponentOverlaysActivate) != MH_OK)
		{
			Log("FixMakeupLag Patch Failed!\n");
			return false;
		}

		// TODO: One or both of these might not be needed now that we are also hooking cTSUICASComponentOverlays::Activate?

		UITabChangeHookReturn = (void*)((DWORD)Addresses::UnknownUITabChange + 6);
		MakeJMP((BYTE*)Addresses::UnknownUITabChange, (DWORD)UITabChangeHook, 6);

		UIMirrorTabChangeHookReturn = (void*)((DWORD)Addresses::UnknownMirrorUITabChange + 6);
		MakeJMP((BYTE*)Addresses::UnknownMirrorUITabChange, (DWORD)UIMirrorTabChangeHook, 6);
	}

	if (ADDRESS_VALID(Addresses::cTSSGSystemOncePerFrameUpdate)) {
		if (MH_CreateHook(Addresses::cTSSGSystemOncePerFrameUpdate, &DetourOncePerFrameUpdate,
			reinterpret_cast<LPVOID*>(&fpOncePerFrameUpdate)) != MH_OK)
		{
			Log("cTSSGSystemOncePerFrameUpdate Patch Failed!\n");
			return false;
		}
		if (MH_EnableHook(Addresses::cTSSGSystemOncePerFrameUpdate) != MH_OK)
		{
			Log("cTSSGSystemOncePerFrameUpdate Patch Failed!\n");
			return false;
		}
	}

	if (Config::Separates4All && ADDRESS_VALID(Addresses::CalculateOutfitPartVisibility)) {
		WriteToMemory((DWORD)Addresses::CalculateOutfitPartVisibility, &separatesPatch, 4);

		WriteToMemory((DWORD)Addresses::CalculateBuyPartVisibility, &separatesBuyPatch, 3);
		WriteToMemory((DWORD)Addresses::CalculateBuyPartVisibility + 0xE, &separatesBuyPatch, 3);
		WriteToMemory((DWORD)Addresses::CalculateBuyPartVisibility + 0xE + 0xE, &separatesBuyPatch, 3);

		WriteToMemory((DWORD)Addresses::CalculateTryOnPartVisibility, &separatesBuyPatch, 3);
		WriteToMemory((DWORD)Addresses::CalculateTryOnPartVisibility + 0xE, &separatesBuyPatch, 3);
		WriteToMemory((DWORD)Addresses::CalculateTryOnPartVisibility + 0xE + 0xE, &separatesBuyPatch, 3);
	}

	if (Config::UIScale && ADDRESS_VALID(Addresses::LegacyCalculateUIScale)) {
		float* pResRef = *(float**)((DWORD)Addresses::LegacyCalculateUIScale + 0x3A);
		float* pRes1080 = *(float**)((DWORD)Addresses::LegacyCalculateUIScale + 0x105);
		float* pRes768 = *(float**)((DWORD)Addresses::LegacyCalculateUIScale + 0x11D);
		float* pRes924 = *(float**)((DWORD)Addresses::LegacyCalculateUIScale + 0x148);

		pResRef[0] = Config::UIScaleResolution;
		pRes1080[0] = Config::UIScaleResolution;
		pRes768[0] = Config::UIScaleResolution;
		pRes924[0] = Config::UIScaleResolution;
	}

	if (Config::FreeZodiac && ADDRESS_VALID(Addresses::CalcZodiacAddress)) {
		static const char jmpChar = 0xEB;
		WriteToMemory((DWORD)Addresses::CalcZodiacAddress, (void*)(&jmpChar), 1);
	}

	if (ADDRESS_VALID(Addresses::cEMVoxModifierModifyEvent)) {
		ModifyVoiceEventHook1Return = (void*)((DWORD)Addresses::cEMVoxModifierModifyEvent + 0x2D + 7);
		ModifyVoiceEventHook2Return = (void*)((DWORD)Addresses::cEMVoxModifierModifyEvent + 0x3B1 + 5);
		MakeJMP((BYTE*)Addresses::cEMVoxModifierModifyEvent + 0x3B1, (DWORD)ModifyVoiceEventHook2, 5);
	}
	
	if (ADDRESS_VALID(Addresses::TSStringLoad)) {
		if (MH_CreateHook(Addresses::TSStringLoad, &DetourTSStringLoad,
			reinterpret_cast<LPVOID*>(&fpTSStringLoad)) != MH_OK)
		{
			Log("TSStringLoad Patch Failed!\n");
			return false;
		}
		if (MH_EnableHook(Addresses::TSStringLoad) != MH_OK)
		{
			Log("TSStringLoad Patch Failed!\n");
			return false;
		}
	}

	if (ADDRESS_VALID(Addresses::LoadUIScript)) {
		if (MH_CreateHook(Addresses::LoadUIScript, &DetourLoadUIScript,
			reinterpret_cast<LPVOID*>(&fpLoadUiScript)) != MH_OK)
		{
			Log("LoadUIScript Patch Failed!\n");
			return false;
		}
		if (MH_EnableHook(Addresses::LoadUIScript) != MH_OK)
		{
			Log("LoadUIScript Patch Failed!\n");
			return false;
		}
	}

	if (ADDRESS_VALID(Addresses::UIMakeMoneyString)) {
		if (MH_CreateHook(Addresses::UIMakeMoneyString, &DetourMakeMoneyString,
			reinterpret_cast<LPVOID*>(&fpMakeMoneyString)) != MH_OK)
		{
			Log("UIMakeMoneyString Patch Failed!\n");
			return false;
		}
		if (MH_EnableHook(Addresses::UIMakeMoneyString) != MH_OK)
		{
			Log("UIMakeMoneyString Patch Failed!\n");
			return false;
		}
	}

	if (ADDRESS_VALID(Addresses::AppendInteractionsForMenu)) {
		if (MH_CreateHook(Addresses::AppendInteractionsForMenu, &DetourAppendInteractionsForMenu,
			reinterpret_cast<LPVOID*>(&fpAppendInteractionsForMenu)) != MH_OK)
		{
			Log("AppendInteractionsForMenu Patch Failed!\n");
			return false;
		}
		if (MH_EnableHook(Addresses::AppendInteractionsForMenu) != MH_OK)
		{
			Log("AppendInteractionsForMenu Patch Failed!\n");
			return false;
		}
	}

	if (ADDRESS_VALID(Addresses::ScenegraphAddGameVersion)) {
		if (MH_CreateHook(Addresses::ScenegraphAddGameVersion, &DetourAddGameVersion,
			reinterpret_cast<LPVOID*>(&fpAddGameVersion)) != MH_OK)
		{
			Log("ScenegraphAddGameVersion Patch Failed!\n");
			return false;
		}
		if (MH_EnableHook(Addresses::ScenegraphAddGameVersion) != MH_OK)
		{
			Log("ScenegraphAddGameVersion Patch Failed!\n");
			return false;
		}
	}

	return true;
}