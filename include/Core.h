#pragma once
#include <string>
#include <unordered_map>
#include "StringId.h"
#include "ts2/lua.h"
#include "LuaCallback.h"

enum class Delegates {
	OnBuildPieMenu = 0
};

class Core {
public:
	static bool Create();
	static bool Initialize();
	std::wstring m_GameDisplayName;
	std::wstring m_UserDataPath;
	static Core* _instance;
	std::unordered_map<StringId, StringId> m_StringOverrides;
	std::unordered_map<uint32_t, uint32_t> m_UIOverrides;

	lua_State* m_MakeMoneyStringLuaState = nullptr;
	int m_MakeMoneyStringLuaCall = LUA_NOREF;

	LuaDelegate m_LuaDelegates[1];
private:
	void CacheUserData();
};