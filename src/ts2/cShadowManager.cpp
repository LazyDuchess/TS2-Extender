#include "ts2/cShadowManager.h"

void cShadowManager::SetShadowVar1(float val) {
	(*(float*)(this + 0x44)) = val;
}

void cShadowManager::SetResolution(int res) {
#if TS2_LC
	(*(int*)(this + 0x34)) = res;
	(*(int*)(this + 0x48)) = res;
#else
	(*(int*)(this + 0x3C)) = res;
	(*(int*)(this + 0x40)) = res;
#endif
}

void cShadowManager::SetIndoorBlur(int blur) {
#if TS2_LC
	(*(int*)(this + 0x2c)) = blur;
#else
	(*(int*)(this + 0x34)) = blur;
#endif
}

void cShadowManager::SetOutdoorBlur(int blur) {
#if TS2_LC
	(*(int*)(this + 0x30)) = blur;
#else
	(*(int*)(this + 0x38)) = blur;
#endif
}