#pragma once
#include "IdManager.h"
#include "ts2/lua.h"
#include <vector>

// Callbacks
class LuaCallback {
public:
	LuaCallback(int luaCall, lua_State* luaState, int priority);
	Id64 m_Id;
	lua_State* m_luaState = nullptr;
	int m_LuaCall = LUA_NOREF;
	int m_Priority = 0;
};

// Delegates contain callbacks!
class LuaDelegate {
public:
	std::vector<LuaCallback> m_Callbacks;
	Id64 AddCallback(LuaCallback callback);
	void RemoveCallback(Id64 callbackId);
	IdManager m_IdManager;
private:
	void Sort();
};