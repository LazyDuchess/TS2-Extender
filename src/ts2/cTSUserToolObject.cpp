#include "ts2/cTSUserToolObject.h"

cTSObject* cTSUserToolObject::GetHeldObject() {
	return (*(cTSObject**)(this + 0xD0));
}