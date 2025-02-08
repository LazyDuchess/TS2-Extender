#pragma once
#include "cCheatCommand.h"

namespace TS2 {
	class cTSCheatSystem {
	private:
		virtual void QueryInterface();
		virtual void AddRef();
		virtual void Release();

		virtual void* AsParser();
	public:
		virtual void RegisterCheat(cCheatCommand* command);
	};
	cTSCheatSystem* CheatSystem();
	void TSRegisterTestingCheat(cCheatCommand* cheat);
}