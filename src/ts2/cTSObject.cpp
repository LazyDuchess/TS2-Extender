#include "ts2/cTSObject.h"

short cTSObject::GetID() {
	int vTableAddr = *(int*)this;
	return ((short(__thiscall*)(cTSObject*)) * (int*)(vTableAddr + 0xBC))(this);
}