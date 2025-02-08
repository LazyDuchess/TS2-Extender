#include "LuaCheatCommand.h"
#include "Logging.h"
#include <stdio.h>

LuaCheatCommand::LuaCheatCommand(const char* name, const char* description, int luaCall, lua_State* luaState) {
	iLuaCall = luaCall;
	sName = _strdup(name);
	sDescription = _strdup(description);
	pLuaState = luaState;
}

const char* LuaCheatCommand::Name() {
	return sName;
}

const char* LuaCheatCommand::Description() {
	return sDescription;
}

void LuaCheatCommand::Execute(void* args) {
	if (pLuaState == nullptr)
	{
		Log("Lua State is NULL!\n");
		return;
	}
	if (iLuaCall == LUA_NOREF) {
		Log("No Lua function stored for cheat.\n");
		return;
	}
	lua_rawgeti(pLuaState, LUA_REGISTRYINDEX, iLuaCall);
	if (lua_pcall(pLuaState, 0, 0, 0) != 0) {
		Log("Error calling Lua cheat: %s\n", lua_tostring(pLuaState, -1));
		lua_pop(pLuaState, 1);
	}
}