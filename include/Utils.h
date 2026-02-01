#pragma once
#include <string>
#include <Windows.h>
#include <stringapiset.h>

inline std::wstring CharToWString(const char* str) {
	int count = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
	if (count <= 0)
		return std::wstring();
	std::wstring result(count - 1, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, str, -1, result.data(), count);
	return result;
}

inline std::string WCharToString(const wchar_t* str) {
	int count = WideCharToMultiByte(CP_UTF8, 0, str, -1, nullptr, 0, nullptr, nullptr);
	if (count <= 0)
		return std::string();
	std::string result(count - 1, L'\0');
	WideCharToMultiByte(CP_UTF8, 0, str, -1, result.data(), count, nullptr, nullptr);
	return result;
}