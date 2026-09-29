#include "ts2/cMaterialParser.h"

void cMaterialParser::SetVariable(int unk, const char* name, const char* value) {
	int vTableAddr = *(int*)this;
	((void(__thiscall*)(cMaterialParser*, int, const char*, const char*)) * (int*)(vTableAddr))(this, name, value);
}

void cMaterialParser::RemoveVariable(int unk, const char* name) {
	int vTableAddr = *(int*)this;
	((void(__thiscall*)(cMaterialParser*, int, const char*)) * (int*)(vTableAddr + 0x4))(this, name);
}