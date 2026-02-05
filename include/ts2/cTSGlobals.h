#pragma once
#include "ts2/cEdithObjectModule.h"

class cTSGlobals {
public:
	static cTSGlobals* GetInstance();
	cEdithObjectModule* ObjectManager();
};