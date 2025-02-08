#include "ts2/cCheatCommand.h"

class LuaCheatCommand : public TS2::cCheatCommand {
public:
	const char* Name();
	const char* Description();
	void Execute(void* args);
};