#pragma once
#define LUA_NOREF (-2)
#define LUA_REGISTRYINDEX (-10000)

struct lua_State {

};

typedef int(_cdecl LUAFUNCTION)(lua_State*);

void lua_pushstring(lua_State* luaState, const char* str);
void lua_rawgeti(lua_State* luaState, int idx, int n);
void lua_settop(lua_State* luaState, int idx);
int lua_pcall(lua_State* luaState, int nargs, int nresults, int errfunc);
void lua_pushvalue(lua_State* luaState, int idx);
const char* lua_tostring(lua_State* luaState, int idx);
int luaL_ref(lua_State* luaState, int t);
double lua_tonumber(lua_State* luaState, int idx);
void lua_pushnumber(lua_State* luaState, double number);
void lua_newtable(lua_State* luaState);

#define lua_pop(L, n) lua_settop(L, -(n)-1)