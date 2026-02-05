#include "ts2/cEdithObjectTestSim.h"

void* cEdithObjectTestSim::GetPerson() {
	return *(void**)(this + 0x8);
}

void* cEdithObjectTestSim::GetObject() {
	return *(void**)(this + 0xC);
}