#pragma once

class cDeviceSetupParam {
public:
	int m_Width;
	int m_Height;
	float m_Unknown;
	int m_RefreshRate;
	void** m_PixelFormat;
	bool IsWindowed();
	void MakeWindowed();
};