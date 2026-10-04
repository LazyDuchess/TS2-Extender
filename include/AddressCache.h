#pragma once
#include <string>
#include <unordered_map>

class AddressCache {
public:
	void* Get(const char* name);
	void Set(const char* name, void* offset);
	void Write(std::wstring path);
	void Read(std::wstring path);
private:
	std::unordered_map<std::string, void*> m_AddressMap;
};