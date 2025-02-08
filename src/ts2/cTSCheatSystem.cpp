#include "ts2/cTSCheatSystem.h"
#include "Addresses.h"

namespace TS2 {
	cTSCheatSystem* CheatSystem() {
		using CheatSystemFunc = cTSCheatSystem *(__stdcall*)();
		CheatSystemFunc func = reinterpret_cast<CheatSystemFunc>(Addresses::GetCheatSystem);
		return func();
	}
	void TSRegisterTestingCheat(cCheatCommand* cheat) {
		using RegisterTestingCheatFunc = void(__stdcall*)(cCheatCommand*);
		RegisterTestingCheatFunc func = reinterpret_cast<RegisterTestingCheatFunc>(Addresses::RegisterTestingCheat);
		func(cheat);
	}
}