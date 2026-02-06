#include "ts2/cTSInteraction.h"

void cTSInteraction::SetStackVars(short vars[]) {
	int vTableAddr = *(int*)this;
	return ((void(__thiscall*)(cTSInteraction*, short[])) * (int*)(vTableAddr + 0x6C))(this, vars);
}

void cTSInteraction::SetTargetObject(cTSObject* targetObject) {
	int vTableAddr = *(int*)this;
	return ((void(__thiscall*)(cTSInteraction*, cTSObject*)) * (int*)(vTableAddr + 0x104))(this, targetObject);
}