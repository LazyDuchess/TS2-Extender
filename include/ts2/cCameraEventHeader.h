#pragma once
#define CAM_MOVE 0xbc17e41c
typedef int cameventid_t;

class cCameraEventHeader {
private:
	void* vTable;
public:
	int m_DeltaX;
	int m_DeltaY;
};