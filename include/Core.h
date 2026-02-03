#pragma once
#include <string>
#include <unordered_map>
#include "StringId.h"

class Core {
public:
	static bool Create();
	static bool Initialize();
	std::wstring m_GameDisplayName;
	std::wstring m_UserDataPath;
	static Core* _instance;
	std::unordered_map<StringId, StringId> m_StringOverrides;
	std::unordered_map<uint32_t, uint32_t> m_UIOverrides;
private:
	void CacheUserData();
};