#include "ts2/cTSUserToolObject.h"

// lowk don't know what it is. not a cTSObject
void* cTSUserToolObject::GetHeldObject() {
	return (*(void**)(this + 0xD0));
}