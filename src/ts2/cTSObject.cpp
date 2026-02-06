#include "ts2/cTSObject.h"

short cTSObject::GetID() {
	int vTableAddr = *(int*)this;
	return ((short(__thiscall*)(cTSObject*)) * (int*)(vTableAddr + 0xBC))(this);
}

bool cTSObject::GetMiscFlag(int flag) {
	int vTableAddr = *(int*)this;
	return ((bool(__thiscall*)(cTSObject*, int)) * (int*)(vTableAddr + 0x64))(this, flag);
}