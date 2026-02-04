#include "ts2/cRZString.h"
#include "Addresses.h"

void cRZString::FromChar(const char* str) {
	((void(__thiscall*)(cRZString*, const char*))Addresses::CRZStringFromChar)(this, str);
}