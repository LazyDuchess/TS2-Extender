#pragma once
#include "cCheatCommand.h"

namespace TS2 {
	class cTSCheatSystem {
	private:
		virtual void fn1();
		virtual void fn2();
		virtual void fn3(); 
		virtual void* AsParser();
	public:
		virtual void RegisterCheat(cCheatCommand* cheat);
	};
	cTSCheatSystem* CheatSystem();
	void TSRegisterTestingCheat(cCheatCommand* cheat);
}