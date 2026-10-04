#include "ts2/cGZWinMgrBase.h"

bool cGZWinMgrBase::IsModal() {
	int vTableAddr = *(int*)this;
	return ((bool(__thiscall*)(cGZWinMgrBase*)) * (int*)(vTableAddr + 0xAC))(this);
}

cIGZWin* cGZWinMgrBase::GZGetFocus() {
	int vTableAddr = *(int*)this;
	return ((cIGZWin*(__thiscall*)(cGZWinMgrBase*)) * (int*)(vTableAddr + 0x94))(this);
}