#include "ts2/cTSPerson.h"
#include "ts2/cTSObject.h"

cTSObject* cTSPerson::AsEdithObject() {
	int vTableAddr = *(int*)this;
	return ((cTSObject*(__thiscall*)(cTSPerson*))*(int*)(vTableAddr + 0x20))(this);
}