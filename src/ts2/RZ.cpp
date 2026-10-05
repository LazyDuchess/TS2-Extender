#include "ts2/RZ.h"
#include "Addresses.h"

namespace RZ {
	cGZWinMgrBase* WM() {
		return ((cGZWinMgrBase * (__stdcall*)())Addresses::WinManager)();
	}
}