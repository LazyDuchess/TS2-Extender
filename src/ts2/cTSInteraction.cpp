#include "ts2/cTSInteraction.h"

void cTSInteraction::SetStackVars(short param0, short param1, short param2, short param3) {
	int vTableAddr = *(int*)this;
	return ((void(__thiscall*)(cTSInteraction*, short, short, short ,short)) * (int*)(vTableAddr + 0x6C))(this, param0, param1, param2, param3);
}

void cTSInteraction::SetTargetObject(cTSObject* targetObject) {
	int vTableAddr = *(int*)this;
	return ((void(__thiscall*)(cTSInteraction*, cTSObject*)) * (int*)(vTableAddr + 0x104))(this, targetObject);
}