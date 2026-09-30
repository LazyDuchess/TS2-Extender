#include "ts2/cIGZWin.h"

bool cIGZWin::ChildDeleteAll() {
	int vTableAddr = *(int*)this;
	return ((bool(__thiscall*)(cIGZWin*)) * (int*)(vTableAddr + 0x5C))(this);
}

cIGZWin* cIGZWin::GetChildWindowFromID(int id) {
	int vTableAddr = *(int*)this;
	return ((cIGZWin*(__thiscall*)(cIGZWin*, int)) * (int*)(vTableAddr + 0x98))(this, id);
}

cIGZWin* cIGZWin::GetChildWindowFromIDRecursive(int id) {
	int vTableAddr = *(int*)this;
	return ((cIGZWin * (__thiscall*)(cIGZWin*, int)) * (int*)(vTableAddr + 0x9C))(this, id);
}

int cIGZWin::SetFlag(winflag_t flag, bool value) {
	int vTableAddr = *(int*)this;
	return ((int(__thiscall*)(cIGZWin*, winflag_t, bool)) * (int*)(vTableAddr + 0x14C))(this, flag, value);
}

int cIGZWin::GetChildCount() {
	int vTableAddr = *(int*)this;
	return ((int(__thiscall*)(cIGZWin*)) * (int*)(vTableAddr + 0x3C))(this);
}

bool cIGZWin::EnumChildren(enumQuery_t query, FNENUMCHILDREN callback, void* userdata) {
	int vTableAddr = *(int*)this;
	return ((int(__thiscall*)(cIGZWin*, enumQuery_t, FNENUMCHILDREN, void*)) * (int*)(vTableAddr + 0x94))(this, query, callback, userdata);
}

int cIGZWin::GetID() {
	int vTableAddr = *(int*)this;
	return ((int(__thiscall*)(cIGZWin*)) * (int*)(vTableAddr + 0x138))(this);
}