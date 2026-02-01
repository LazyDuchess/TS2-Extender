#pragma once
#include <string>

class Core {
public:
	static bool Create();
	static bool Initialize();
	std::wstring m_GameDisplayName;
	std::wstring m_UserDataPath;
	static Core* _instance;
private:
	void CacheUserData();
};