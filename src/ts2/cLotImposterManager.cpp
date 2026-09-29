#include "ts2/cLotImposterManager.h"

void nTSSG::cLotImposterManager::SetWidth(int width) {
#if TS2_LC
	(*(int*)(this + 0xA4)) = width;
#else
	(*(int*)(this + 0x9C)) = width;
#endif
}

void nTSSG::cLotImposterManager::SetHeight(int height) {
#if TS2_LC
	(*(int*)(this + 0xA8)) = height;
#else
	(*(int*)(this + 0xA0)) = height;
#endif
}

void nTSSG::cLotImposterManager::SetBlur(int blur) {
#if TS2_LC
	(*(int*)(this + 0x138)) = blur;
#else
	(*(int*)(this + 0x128)) = blur;
#endif
}

void nTSSG::cLotImposterManager::SetSliceResolution(int sliceRes) {
#if TS2_LC
	(*(int*)(this + 0x14C)) = sliceRes;
#else
	(*(int*)(this + 0x13C)) = sliceRes;
#endif
}