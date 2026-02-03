#pragma once
#include "cUserInput.h"

class cTSString {
public:
	cUserInput* GetString();

	short GetInstance();
	short GetIndex();
	int GetGroup();

	void SetInstance(short instance);
	void SetIndex(short index);
	void SetGroup(int group);
};