#include "ts2/cDeviceSetupParam.h"

bool cDeviceSetupParam::IsWindowed() {
	return m_PixelFormat == nullptr;
}
void cDeviceSetupParam::MakeWindowed() {
	m_Width = 0;
	m_Height = 0;
	m_Unknown = 0.0f;
	m_RefreshRate = 0;
	m_PixelFormat = nullptr;
}