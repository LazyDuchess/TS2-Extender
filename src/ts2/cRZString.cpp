#include "ts2/cRZString.h"
#include "Addresses.h"

void cRZString::FromChar(const char* str) {
	((void(__thiscall*)(cRZString*, const char*))Addresses::CRZStringFromChar)(this, str);
}

const char* cRZString::GetString() {
	if (IsEmbedded())
		return (const char*)(this + 0x4);
	return *(const char**)(this + 0x4);
}

bool cRZString::IsEmbedded() {
	return (*(char*)(this + 0x18)) == 0x0F;
}