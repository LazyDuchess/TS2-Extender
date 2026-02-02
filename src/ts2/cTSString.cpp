#include "ts2/cTSString.h"

cUserInput* cTSString::GetString() {
	return (cUserInput*)(this + 0x30 + 0x40 - 0x28 - 0x14 - 0x9C);
}

int cTSString::GetInstance() {
	return (*(int*)(this + 0x14));
}

int cTSString::GetIndex() {
	return (*(int*)(this + 0x18));
}

int cTSString::GetGroup() {
	return (*(int*)(this + 0x1C));
}