#include "ts2/lua.h"
#include "Addresses.h"

void lua_pushstring(lua_State* luaState, const char* str) {
	using LuaPushStringFunc = void(__cdecl*)(lua_State*, const char*);
	LuaPushStringFunc func = reinterpret_cast<LuaPushStringFunc>(Addresses::LuaPushString);
	func(luaState, str);
}

void lua_rawgeti(lua_State* luaState, int idx, int n) {
	using LuaRawGetIFunc = void(__cdecl*)(lua_State*, int, int);
	LuaRawGetIFunc func = reinterpret_cast<LuaRawGetIFunc>(Addresses::LuaRawGetI);
	func(luaState, idx, n);
}

void lua_settop(lua_State* luaState, int idx) {
	using LuaSetTopFunc = void(__cdecl*)(lua_State*, int);
	LuaSetTopFunc func = reinterpret_cast<LuaSetTopFunc>(Addresses::LuaSetTop);
	func(luaState, idx);
}

int lua_pcall(lua_State* luaState, int nargs, int nresults, int errfunc) {
	using LuaPCallFunc = int(__cdecl*)(lua_State*, int, int, int);
	LuaPCallFunc func = reinterpret_cast<LuaPCallFunc>(Addresses::LuaPCall);
	return func(luaState, nargs, nresults, errfunc);
}

void lua_pushvalue(lua_State* luaState, int idx) {
	using LuaPushValueFunc = void(__cdecl*)(lua_State*, int);
	LuaPushValueFunc func = reinterpret_cast<LuaPushValueFunc>(Addresses::LuaPushValue);
	func(luaState, idx);
}

const char* lua_tostring(lua_State* luaState, int idx) {
	using LuaToStringFunc = const char*(__cdecl*)(lua_State*, int);
	LuaToStringFunc func = reinterpret_cast<LuaToStringFunc>(Addresses::LuaToString);
	return func(luaState, idx);
}

int luaL_ref(lua_State* luaState, int t) {
	using LuaLRefFunc = int (__cdecl*)(lua_State*, int);
	LuaLRefFunc func = reinterpret_cast<LuaLRefFunc>(Addresses::LuaLRef);
	return func(luaState, t);
}

double lua_tonumber(lua_State* luaState, int idx) {
	using LuaToNumberFunc = double (__cdecl*)(lua_State*, int);
	LuaToNumberFunc func = reinterpret_cast<LuaToNumberFunc>(Addresses::LuaToNumber);
	return func(luaState, idx);
}