#include "ts2/nsGZWinUtils.h"
#include "Addresses.h"

bool nsGZWinUtils::SetWindowEnabled(cIGZWin* win, int windowId, bool enabled) {
	return ((bool(__cdecl*)(cIGZWin*, int, bool))Addresses::SetWindowEnabled)(win, windowId, enabled);
}