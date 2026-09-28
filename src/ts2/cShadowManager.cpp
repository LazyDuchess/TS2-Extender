#include "ts2/cShadowManager.h"

void cShadowManager::SetShadowVar1(float val) {
	(*(float*)(this + 0x44)) = val;
}

void cShadowManager::SetResolution(int res) {
	(*(int*)(this + 0x3C)) = res;
	(*(int*)(this + 0x40)) = res;
}

void cShadowManager::SetIndoorBlur(int blur) {
	(*(int*)(this + 0x34)) = blur;
}

void cShadowManager::SetOutdoorBlur(int blur) {
	(*(int*)(this + 0x38)) = blur;
}