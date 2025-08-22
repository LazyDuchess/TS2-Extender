#include "LuaExtensions.h"
#include "ts2/cIGZLua5Thread.h"
#include "MinHook.h"
#include "Addresses.h"
#include "Logging.h"
#include "ts2/CheatSystem.h"
#include "LuaCheatCommand.h"
#include "PatchVersion.h"

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

	static int __cdecl LuaRegisterTestingCheat(lua_State* luaState) {
		const char* cheatName = lua_tostring(luaState, 1);
		const char* cheatDesc = lua_tostring(luaState, 2);
		lua_pushvalue(luaState, 3);
		int executeRef = luaL_ref(luaState, LUA_REGISTRYINDEX);
		LuaCheatCommand* luaCheat = new LuaCheatCommand(cheatName, cheatDesc, executeRef, luaState);
		TS2::TSRegisterTestingCheat(luaCheat);
		return 0;
	}

	static int __cdecl LuaGetExecutableDirectory(lua_State* luaState) {
		lua_pushstring(luaState, GetProcessDirectory().c_str());
		return 1;
	}

	static int __cdecl LuaGetTS2ExtenderVersion(lua_State* luaState) {
		lua_pushstring(luaState, Version);
	}

	static bool __cdecl DetourRegisterPrimitiveSupportLuaCommands(TS2::cIGZLua5Thread* luaThread) {
		bool res = fpRegisterPrimitiveSupportLuaCommands(luaThread);
		if (luaThread != NULL) {
			luaThread->Register(&LuaGetExecutableDirectory, "GetExecutableDirectory");
			luaThread->Register(&LuaRegisterTestingCheat, "RegisterTestingCheat");
			luaThread->Register(&LuaRegisterTestingCheat, "GetTS2ExtenderVersion");
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