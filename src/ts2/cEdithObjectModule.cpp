#include "ts2/cEdithObjectModule.h"

cTSObject* cEdithObjectModule::GetObjectFromID(short objectId) {
	int vTableAddr = *(int*)this;
	return ((cTSObject*(__thiscall*)(cEdithObjectModule*, short)) * (int*)(vTableAddr + 0x74))(this, objectId);
}