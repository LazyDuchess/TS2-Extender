#include "ts2/cGZFramework.h"
#include "Addresses.h"

bool cGZFramework::QueryInterface(iid_t id, cIGZUnknown** out) {
	int vTableAddr = *(int*)this;
	return ((bool(__thiscall*)(cGZFramework*, iid_t, cIGZUnknown**)) * (int*)(vTableAddr))(this, id, out);
}

int cGZFramework::AddRef() {
	int vTableAddr = *(int*)this;
	return ((int(__thiscall*)(cGZFramework*)) * (int*)(vTableAddr + 0x4))(this);
}

int cGZFramework::Release() {
	int vTableAddr = *(int*)this;
	return ((int(__thiscall*)(cGZFramework*)) * (int*)(vTableAddr + 0x8))(this);
}

int cIGZApp::Release() {
	int vTableAddr = *(int*)this;
	return ((int(__thiscall*)(cIGZApp*)) * (int*)(vTableAddr + 0x8))(this);
}

HWND cIGZApp::GetMainHWND() {
	int vTableAddr = *(int*)this;
	return ((HWND(__thiscall*)(cIGZApp*)) * (int*)(vTableAddr + 0x14))(this);
}

cGZFramework* RZGetFramework() {
	return ((cGZFramework * (__stdcall*)())Addresses::GetFramework)();
}