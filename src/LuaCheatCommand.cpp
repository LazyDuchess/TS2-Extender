#include "LuaCheatCommand.h"
#include <stdio.h>

const char* LuaCheatCommand::Name() {
	return "luaTestCheat";
}

const char* LuaCheatCommand::Description() {
	return "This is for testing lua cheats.";
}

void LuaCheatCommand::Execute(void* args) {
	printf("Cheat called!\n");
}