#pragma once
#include "ts2/cTSObject.h"

class cTSInteraction {
public:
	void SetStackVars(short vars[]);
	void SetTargetObject(cTSObject* targetObject);
};