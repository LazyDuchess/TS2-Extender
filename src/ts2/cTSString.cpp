#include "ts2/cTSString.h"

cUserInput* cTSString::GetString() {
	return (cUserInput*)(this + 0x30 + 0x40 - 0x28 - 0x14 - 0x9C);
}

short cTSString::GetInstance() {
	return (*(short*)(this + 0x2A));
}

short cTSString::GetIndex() {
	return (*(short*)(this + 0x28));
}

int cTSString::GetGroup() {
	return (*(int*)(this + 0x24));
}

void cTSString::SetInstance(short instance) {
	(*(short*)(this + 0x2A)) = instance;
}

void cTSString::SetIndex(short index) {
	(*(short*)(this + 0x28)) = index;
}

void cTSString::SetGroup(int group) {
	(*(int*)(this + 0x24)) = group;
}