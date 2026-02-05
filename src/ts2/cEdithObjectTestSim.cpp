#include "ts2/cEdithObjectTestSim.h"
#include "ts2/cTSPerson.h"
#include "ts2/cTSObject.h"

cTSPerson* cEdithObjectTestSim::GetPerson() {
	return *(cTSPerson**)(this + 0x8);
}

cTSObject** cEdithObjectTestSim::GetObject() {
	return *(cTSObject**)(this + 0xC);
}