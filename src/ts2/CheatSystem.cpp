#include "ts2/CheatSystem.h"
#include "Addresses.h"

namespace TS2 {
	void TSRegisterTestingCheat(cCheatCommand* cheat) {
		using RegisterTestingCheatFunc = void(__cdecl*)(cCheatCommand*);
		RegisterTestingCheatFunc func = reinterpret_cast<RegisterTestingCheatFunc>(Addresses::RegisterTestingCheat);
		func(cheat);
	}
}