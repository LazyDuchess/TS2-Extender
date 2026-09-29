#include "ts2/CheatSystem.h"
#include "Addresses.h"

namespace TS2 {
	cTSCheatSystem* CheatSystem() {
		return ((cTSCheatSystem*(__stdcall*)())Addresses::GetCheatSystem)();
	}

	void TSRegisterTestingCheat(cCheatCommand* cheat) {
		using RegisterTestingCheatFunc = void(__cdecl*)(cCheatCommand*);
		RegisterTestingCheatFunc func = reinterpret_cast<RegisterTestingCheatFunc>(Addresses::RegisterTestingCheat);
		func(cheat);
	}

	void cTSCheatSystem::ExecuteCommand(const char* command) {
		cTSCheatParser* parser = this->AsParser();
		((void (__cdecl*)(cTSCheatParser*, const char*))Addresses::CheatParserExecuteCommand)(parser, command);
	}
}