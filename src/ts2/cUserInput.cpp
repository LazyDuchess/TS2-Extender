#pragma once
#include "ts2/cUserInput.h"
#include <cstdlib>
#include <string.h>

cUserInput* cUserInput::GetInstance() {
	return *m_GlobalUserInputPtr;
}

const char* cUserInput::GetString() {
	if (IsStringEmbedded())
		return (const char*)(this + 0x9C);
	return *(const char**)(this + 0x9C);
}

bool cUserInput::IsStringEmbedded() {
	return (*(char*)(this + 0x9C + 0x14)) == 0x0F;
}

void cUserInput::SetString(const char* str) {
	(*(char*)(this + 0x9C + 0x14)) = 0x0F;
	size_t len = strlen(str);
	memcpy((this + 0x9C), str, len);
	(*(char*)(this + 0x9C + len + 1)) = 0x00;
}