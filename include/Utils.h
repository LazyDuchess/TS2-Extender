#pragma once
#include <string>
#include <Windows.h>
#include <stringapiset.h>

inline std::wstring CharToWString(const char* str) {
    int count = MultiByteToWideChar(CP_UTF8, 0, str, -1, nullptr, 0);
    if (count <= 0)
        return {};

    std::wstring result(count, L'\0');

    MultiByteToWideChar(
        CP_UTF8, 0,
        str, -1,
        result.data(), count
    );

    result.pop_back();
    return result;
}

inline std::string WCharToString(const wchar_t* str) {
    int count = WideCharToMultiByte(CP_UTF8, 0, str, -1, nullptr, 0, nullptr, nullptr);
    if (count <= 0)
        return {};

    std::string result(count, '\0');

    WideCharToMultiByte(
        CP_UTF8, 0,
        str, -1,
        result.data(), count,
        nullptr, nullptr
    );

    result.pop_back();
    return result;
}