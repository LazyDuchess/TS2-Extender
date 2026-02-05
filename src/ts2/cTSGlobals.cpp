#include "ts2/cTSGlobals.h"
#include "Addresses.h"

cTSGlobals* cTSGlobals::GetInstance() {
	return ((cTSGlobals * (__stdcall*)())Addresses::TSGlobals)();
}

cEdithObjectModule* cTSGlobals::ObjectManager() {
	int vTableAddr = *(int*)this;
	return ((cEdithObjectModule * (__thiscall*)(cTSGlobals*)) * (int*)(vTableAddr + 0x68))(this);
}