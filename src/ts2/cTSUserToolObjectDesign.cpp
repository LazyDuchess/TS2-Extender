#include "ts2/cTSUserToolObjectDesign.h"
#if TS2_LC
#define OFFSET_PRICE 0x54
#else
#define OFFSET_PRICE 0x58
#endif

int cTSUserToolObjectDesign::GetPrice() {
	return *(int*)(this + OFFSET_PRICE);
}

void cTSUserToolObjectDesign::SetPrice(int price) {
	(*(int*)(this + OFFSET_PRICE)) = price;
}