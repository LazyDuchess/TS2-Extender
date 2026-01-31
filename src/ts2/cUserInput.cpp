#pragma once
#include "ts2/cUserInput.h"

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