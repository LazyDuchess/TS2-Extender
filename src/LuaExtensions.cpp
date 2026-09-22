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
#include <filesystem>
#include <algorithm>
#include <fstream>
#include "scan.h"
#include <Windows.h>
#define KEY_COUNT 256

namespace LuaExtensions {
	typedef bool(__cdecl* REGISTERPRIMITIVESUPPORTLUACOMMANDS)(TS2::cIGZLua5Thread*);
	static REGISTERPRIMITIVESUPPORTLUACOMMANDS fpRegisterPrimitiveSupportLuaCommands = NULL;
	typedef void(__stdcall* REGISTERTSSGCHEATS)();
	static REGISTERTSSGCHEATS fpRegisterTSSGCheats = NULL;

	static SHORT currentInputState[KEY_COUNT]{};
	static SHORT lastInputState[KEY_COUNT]{};

	void FrameUpdate() {
		memcpy(lastInputState, currentInputState, sizeof(currentInputState));
		for (int i = 0; i < KEY_COUNT; i++) {
			currentInputState[i] = GetAsyncKeyState(i);
		}
	}

	static std::string GetProcessDirectory() {
		wchar_t path[MAX_PATH];
		if (GetModuleFileNameW(NULL, path, MAX_PATH)) {
			std::string dir = WCharToString(path);
			size_t pos = dir.find_last_of("\\/");
			if (pos != std::string::npos) {
				return dir.substr(0, pos);
			}
		}
		return "";
	}

	// KBM_GetKeyDown(number vk)
	static int __cdecl LuaGetKeyDown(lua_State* luaState) {
		int vk = static_cast<int>(lua_tonumber(luaState, 1));
		bool res = (currentInputState[vk] & 0x8000) && !(lastInputState[vk] & 0x8000);
		lua_pushboolean(luaState, res ? 1 : 0);
		return 1;
	}

	// KBM_GetKey(number vk)
	static int __cdecl LuaGetKeyHeld(lua_State* luaState) {
		int vk = static_cast<int>(lua_tonumber(luaState, 1));
		bool res = (currentInputState[vk] & 0x8000);
		lua_pushboolean(luaState, res ? 1 : 0);
		return 1;
	}

	// KBM_GetKeyUp(number vk)
	static int __cdecl LuaGetKeyUp(lua_State* luaState) {
		int vk = static_cast<int>(lua_tonumber(luaState, 1));
		bool res = !(currentInputState[vk] & 0x8000) && (lastInputState[vk] & 0x8000);
		lua_pushboolean(luaState, res ? 1 : 0);
		return 1;
	}

	// GetCASLot()
	static int __cdecl LuaGetCASLot(lua_State* luaState) {
		lua_pushstring(luaState, Addresses::CASLotName);
		return 1;
	}

	// GetYACASLot()
	static int __cdecl LuaGetYACASLot(lua_State* luaState) {
		lua_pushstring(luaState, Addresses::YACASLotName);
		return 1;
	}

	// SetCASLot(string lot)
	static int __cdecl LuaSetCASLot(lua_State* luaState) {
		const char* lot = lua_tostring(luaState, 1);
		size_t lotLen = std::strlen(lot);
		if (lotLen > 7) {
			Log("Lua: SetCASLot FAILED! Lot name must be 7 characters or less.\n");
			return 0;
		}
		WriteToMemory((DWORD)Addresses::CASLotName, (void*)lot, lotLen + 1);
		return 0;
	}

	// SetYACASLot(string lot)
	static int __cdecl LuaSetYACASLot(lua_State* luaState) {
		const char* lot = lua_tostring(luaState, 1);
		size_t lotLen = std::strlen(lot);
		if (lotLen > 7) {
			Log("Lua: SetYACASLot FAILED! Lot name must be 7 characters or less.\n");
			return 0;
		}
		WriteToMemory((DWORD)Addresses::YACASLotName, (void*)lot, lotLen + 1);
		return 0;
	}

	// EnsureDirectory(string path)
	static int __cdecl LuaEnsureDirectory(lua_State* luaState) {
		const char* path = lua_tostring(luaState, 1);

		std::filesystem::path fPath = std::filesystem::u8path(path);

		std::filesystem::create_directories(fPath);
		return 0;
	}

	// {string, string...} GetFilesInDirectory(string path, string extension)
	static int __cdecl LuaGetFilesInDirectory(lua_State* luaState) {
		const char* path = lua_tostring(luaState, 1);
		const char* ext = lua_tostring(luaState, 2);

		std::string checkExtension = "";
		if (ext != nullptr)
			checkExtension = std::string(ext);

		std::filesystem::path fPath = std::filesystem::u8path(path);

		std::transform(checkExtension.begin(), checkExtension.end(), checkExtension.begin(), ::tolower);

		lua_newtable(luaState);

		int index = 1;

		for (const auto& entry : std::filesystem::recursive_directory_iterator(fPath)) {
			if (entry.is_regular_file()) {
				std::string fext = entry.path().extension().u8string();
				std::transform(fext.begin(), fext.end(), fext.begin(), ::tolower);

				if (fext == checkExtension || checkExtension == "")
				{
					lua_pushnumber(luaState, index);
					lua_pushstring(luaState, entry.path().u8string().c_str());
					lua_settable(luaState, -3);
					index++;
				}
			}
		}

		return 1;
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

	// string id AddGameCallback(number delegate, function callback, number priority)
	static int __cdecl LuaAddGameCallback(lua_State* luaState) {
		int delegate = lua_tonumber(luaState, 1);
		int priority = lua_tonumber(luaState, 3);
		lua_pushvalue(luaState, 2);
		int executeRef = luaL_ref(luaState, LUA_REGISTRYINDEX);
		Id64 result = Core::_instance->m_LuaDelegates[delegate].AddCallback({ executeRef, luaState, priority });
		lua_pushstring(luaState, std::to_string(result.m_Value).c_str());
		return 1;
	}

	// RemoveGameCallback(number delegate, string id)
	static int __cdecl LuaRemoveGameCallback(lua_State* luaState) {
		int delegate = lua_tonumber(luaState, 1);
		std::string idString = lua_tostring(luaState, 2);
		Id64 callbackId = Id64(static_cast<uint64_t>(std::stoll(idString, nullptr)));
		Core::_instance->m_LuaDelegates[delegate].RemoveCallback(callbackId);
		return 0;
	}

	// string ReadFile(string path)
	static int __cdecl LuaReadFile(lua_State* luaState) {
		const char* path = lua_tostring(luaState, 1);
		std::filesystem::path fPath = std::filesystem::u8path(path);

		if (!std::filesystem::exists(fPath)) {
			lua_pushstring(luaState, "");
			return 1;
		}

		std::ifstream file(fPath, std::ios::in | std::ios::binary);

		if (!file)
			throw std::runtime_error("Failed to open file!");

		std::string fstr = std::string(
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
		);

		file.close();

		lua_pushstring(luaState, fstr.c_str());
		return 1;
	}

	// WriteFile(string path, string contents)
	static int __cdecl LuaWriteFile(lua_State* luaState) {
		const char* path = lua_tostring(luaState, 1);
		const char* content = lua_tostring(luaState, 2);

		std::filesystem::path fPath = std::filesystem::u8path(path);

		std::ofstream file(path, std::ios::binary | std::ios::out | std::ios::trunc);

		if (!file)
			throw std::runtime_error("Failed to open file!");

		file.write(content, strlen(content));

		file.close();
		return 0;
	}

	static int __cdecl LuaSetLoadUIScriptDebug(lua_State* luaState) {
		Core::_instance->m_LoadUIScriptDebug = lua_toboolean(luaState, 1) != 0;
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
			luaThread->Register(&LuaEnsureDirectory, "EnsureDirectory");
			luaThread->Register(&LuaGetFilesInDirectory, "GetFilesInDirectory");
			luaThread->Register(&LuaReadFile, "ReadFile");
			luaThread->Register(&LuaWriteFile, "WriteFile");
			luaThread->Register(&LuaSetLoadUIScriptDebug, "SetLoadUIScriptDebug");
			luaThread->Register(&LuaSetCASLot, "SetCASLot");
			luaThread->Register(&LuaSetYACASLot, "SetYACASLot");
			luaThread->Register(&LuaGetCASLot, "GetCASLot");
			luaThread->Register(&LuaGetYACASLot, "GetYACASLot");
			luaThread->Register(&LuaGetKeyDown, "KBM_GetKeyDown");
			luaThread->Register(&LuaGetKeyHeld, "KBM_GetKey");
			luaThread->Register(&LuaGetKeyUp, "KBM_GetKeyUp");
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