#pragma once
#include "cCheatCommand.h"

class cTSCheatParser {
};

namespace TS2 {
	class cTSCheatSystem {
	private:
		virtual void fn1();
		virtual void fn2();
		virtual void fn3(); 
		virtual cTSCheatParser* AsParser();
	public:
		virtual void RegisterCheat(cCheatCommand* cheat);
		void ExecuteCommand(const char* command);
	};
	cTSCheatSystem* CheatSystem();
	void TSRegisterTestingCheat(cCheatCommand* cheat);
}