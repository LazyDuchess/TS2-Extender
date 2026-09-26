#include "ts2/cShadow.h"
#include "Addresses.h"

bool cShadow::IsOutside() {
	return ((bool * (__thiscall*)(cShadow*))Addresses::ShadowIsOutside)(this);
}