#pragma once
struct lua_State {

};

typedef int(_cdecl LUAFUNCTION)(lua_State*);

void lua_pushstring(lua_State* luaState, const char* str);