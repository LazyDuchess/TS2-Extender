#include "ts2/lua.h"
#include "Addresses.h"

void lua_pushstring(lua_State* luaState, const char* str) {
	using LuaPushStringFunc = void(__cdecl*)(lua_State*, const char*);
	LuaPushStringFunc func = reinterpret_cast<LuaPushStringFunc>(Addresses::LuaPushString);
	func(luaState, str);
}