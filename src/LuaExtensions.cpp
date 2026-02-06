#include "LuaExtensions.h"
#include "ts2/cIGZLua5Thread.h"
#include "MinHook.h"
#include "Addresses.h"
#include "Logging.h"
#include "ts2/CheatSystem.h"
#include "LuaCheatCommand.h"
#include "PatchVersion.h"
#include "ts2/cUserInput.h"
#include "Core.h"
#include "Utils.h"

namespace LuaExtensions {
	typedef bool(__cdecl* REGISTERPRIMITIVESUPPORTLUACOMMANDS)(TS2::cIGZLua5Thread*);
	static REGISTERPRIMITIVESUPPORTLUACOMMANDS fpRegisterPrimitiveSupportLuaCommands = NULL;
	typedef void(__stdcall* REGISTERTSSGCHEATS)();
	static REGISTERTSSGCHEATS fpRegisterTSSGCheats = NULL;

	static std::string GetProcessDirectory() {
		char path[MAX_PATH];
		if (GetModuleFileName(NULL, path, MAX_PATH)) {
			std::string dir(path);
			size_t pos = dir.find_last_of("\\/");
			if (pos != std::string::npos) {
				return dir.substr(0, pos);
			}
		}
		return "";
	}

	static int __cdecl LuaRegisterCheat(lua_State* luaState) {
		const char* cheatName = lua_tostring(luaState, 1);
		const char* cheatDesc = lua_tostring(luaState, 2);
		lua_pushvalue(luaState, 3);
		int executeRef = luaL_ref(luaState, LUA_REGISTRYINDEX);
		LuaCheatCommand* luaCheat = new LuaCheatCommand(cheatName, cheatDesc, executeRef, luaState);
		TS2::cTSCheatSystem* cheatSystem = TS2::CheatSystem();
		cheatSystem->RegisterCheat(luaCheat);
		return 0;
	}

	static int __cdecl LuaGetUserDirectory(lua_State* luaState) {
		std::string utf8Dir = WCharToString(Core::_instance->m_UserDataPath.c_str());
		lua_pushstring(luaState, utf8Dir.c_str());
		return 1;
	}

	static int __cdecl LuaGetExecutableDirectory(lua_State* luaState) {
		lua_pushstring(luaState, GetProcessDirectory().c_str());
		return 1;
	}

	static int __cdecl LuaGetTS2ExtenderVersion(lua_State* luaState) {
		lua_pushstring(luaState, Version);
		return 1;
	}

	static int __cdecl LuaGetUserInput(lua_State* luaState) {
		cUserInput* userInput = cUserInput::GetInstance();
		if (userInput == nullptr)
		{
			lua_pushstring(luaState, "");
			return 1;
		}
		const char* userString = userInput->GetString();
		if (userString == nullptr) {
			lua_pushstring(luaState, "");
			return 1;
		}
		lua_pushstring(luaState, userString);
		return 1;
	}

	// OverrideString(originalindex, originalinstance, originalgroup, repindex, repinstance, repgroup)
	static int __cdecl LuaOverrideString(lua_State* luaState) {

		uint16_t oIndex = static_cast<uint16_t>(lua_tonumber(luaState, 1));
		uint16_t oInstance = static_cast<uint16_t>(lua_tonumber(luaState, 2));
		uint32_t oGroup = static_cast<uint32_t>(lua_tonumber(luaState, 3));

		uint16_t nIndex = static_cast<uint16_t>(lua_tonumber(luaState, 4));
		uint16_t nInstance = static_cast<uint16_t>(lua_tonumber(luaState, 5));
		uint32_t nGroup = static_cast<uint32_t>(lua_tonumber(luaState, 6));

		Core::_instance->m_StringOverrides[StringId(oIndex, oInstance, oGroup)] = StringId(nIndex, nInstance, nGroup);
		return 0;
	}

	// ClearStringOverride(originalindex, originalinstance, originalgroup)
	static int __cdecl LuaClearStringOverride(lua_State* luaState) {

		uint16_t oIndex = static_cast<uint16_t>(lua_tonumber(luaState, 1));
		uint16_t oInstance = static_cast<uint16_t>(lua_tonumber(luaState, 2));
		uint32_t oGroup = static_cast<uint32_t>(lua_tonumber(luaState, 3));

		StringId strId = StringId(oIndex, oInstance, oGroup);
		auto it = Core::_instance->m_StringOverrides.find(strId);

		if (it != Core::_instance->m_StringOverrides.end()) {
			Core::_instance->m_StringOverrides.erase(it);
		}
		return 0;
	}

	// OverrideUI(originalInstance, newInstance)
	static int __cdecl LuaOverrideUI(lua_State* luaState) {
		uint32_t oUI = static_cast<uint32_t>(lua_tonumber(luaState, 1));

		uint32_t nUI = static_cast<uint32_t>(lua_tonumber(luaState, 2));

		Core::_instance->m_UIOverrides[oUI] = nUI;
		return 0;
	}

	// ClearUIOverride(originalInstance)
	static int __cdecl LuaClearUIOverride(lua_State* luaState) {
		uint32_t oUI = static_cast<uint32_t>(lua_tonumber(luaState, 1));

		auto it = Core::_instance->m_UIOverrides.find(oUI);

		if (it != Core::_instance->m_UIOverrides.end()) {
			Core::_instance->m_UIOverrides.erase(it);
		}
		return 0;
	}

	// OverrideMakeMoneyString(function)
	// function return: string
	// function params: money (number)
	static int __cdecl LuaOverrideMakeMoneyString(lua_State* luaState) {
		lua_pushvalue(luaState, 1);
		int executeRef = luaL_ref(luaState, LUA_REGISTRYINDEX);
		Core::_instance->m_MakeMoneyStringLuaCall = executeRef;
		return 0;
	}

	static int __cdecl LuaClearMakeMoneyStringOverride(lua_State* luaState) {
		Core::_instance->m_MakeMoneyStringLuaCall = LUA_NOREF;
		return 0;
	}

	// number id AddGameCallback(number delegate, function callback, number priority)
	static int __cdecl LuaAddGameCallback(lua_State* luaState) {
		int delegate = lua_tonumber(luaState, 1);
		int priority = lua_tonumber(luaState, 3);
		lua_pushvalue(luaState, 2);
		int executeRef = luaL_ref(luaState, LUA_REGISTRYINDEX);
		Id64 result = Core::_instance->m_LuaDelegates[delegate].AddCallback({ executeRef, luaState, priority });
		lua_pushnumber(luaState, static_cast<double>(result.m_Value));
		return 1;
	}

	// RemoveGameCallback(number delegate, number id)
	static int __cdecl LuaRemoveGameCallback(lua_State* luaState) {
		int delegate = lua_tonumber(luaState, 1);
		Id64 callbackId = Id64(static_cast<uint64_t>(lua_tonumber(luaState, 2)));
		Core::_instance->m_LuaDelegates[delegate].RemoveCallback(callbackId);
		return 0;
	}

	static bool __cdecl DetourRegisterPrimitiveSupportLuaCommands(TS2::cIGZLua5Thread* luaThread) {
		Core::_instance->m_LuaState = luaThread->GetLuaState();
		bool res = fpRegisterPrimitiveSupportLuaCommands(luaThread);
		if (luaThread != NULL) {
			luaThread->Register(&LuaGetExecutableDirectory, "GetExecutableDirectory");
			luaThread->Register(&LuaGetUserDirectory, "GetUserDirectory");
			luaThread->Register(&LuaRegisterCheat, "RegisterCheat");
			luaThread->Register(&LuaGetTS2ExtenderVersion, "GetTS2ExtenderVersion");
			luaThread->Register(&LuaGetUserInput, "GetUserInput");
			luaThread->Register(&LuaOverrideString, "OverrideString");
			luaThread->Register(&LuaClearStringOverride, "ClearStringOverride");
			luaThread->Register(&LuaOverrideUI, "OverrideUI");
			luaThread->Register(&LuaClearUIOverride, "ClearUIOverride");
			luaThread->Register(&LuaOverrideMakeMoneyString, "OverrideMakeMoneyString");
			luaThread->Register(&LuaClearMakeMoneyStringOverride, "ClearMakeMoneyStringOverride");
			luaThread->Register(&LuaAddGameCallback, "AddGameCallback");
			luaThread->Register(&LuaRemoveGameCallback, "RemoveGameCallback");
		}
		return res;
	}

	static void __stdcall DetourRegisterTSSGCheats() {
		Log("Register TSSG cheats!\n");
		fpRegisterTSSGCheats();
	}

	bool Initialize() {
		if (MH_CreateHook(Addresses::RegisterLuaCommands, &DetourRegisterPrimitiveSupportLuaCommands,
			reinterpret_cast<LPVOID*>(&fpRegisterPrimitiveSupportLuaCommands)) != MH_OK)
		{
			return false;
		}
		if (MH_EnableHook(Addresses::RegisterLuaCommands) != MH_OK)
		{
			return false;
		}
		if (MH_CreateHook(Addresses::RegisterTSSGCheats, &DetourRegisterTSSGCheats,
			reinterpret_cast<LPVOID*>(&fpRegisterTSSGCheats)) != MH_OK)
		{
			return false;
		}
		if (MH_EnableHook(Addresses::RegisterTSSGCheats) != MH_OK)
		{
			return false;
		}
		return true;
	}
}