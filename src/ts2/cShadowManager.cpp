#include "ts2/cShadowManager.h"

void cShadowManager::SetShadowVar1(float val) {
	(*(float*)(this + 0x44)) = val;
}