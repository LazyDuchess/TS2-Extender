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
	if (!IsStringEmbedded()) {
		// idk
		free((void*)GetString());
	}
	// idk
	(*(char*)(this + 0x9C + 0x14)) = 0x7F;

	size_t len = strlen(str) + 1;
	void* newMem = malloc(len);
	memcpy(newMem, str, len);

	(*(int*)(this + 0x9C)) = (int)newMem;
}