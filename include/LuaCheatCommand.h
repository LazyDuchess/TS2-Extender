#include "ts2/cCheatCommand.h"
#include "ts2/lua.h"

class LuaCheatCommand : public TS2::cCheatCommand {
public:
	LuaCheatCommand(const char* name, const char* description, int luaCall, lua_State* luaState);
	const char* Name();
	const char* Description();
	void Execute(void* args);
private:
	lua_State* pLuaState = nullptr;
	int iLuaCall = LUA_NOREF;
	const char* sName = "";
	const char* sDescription = "";
};