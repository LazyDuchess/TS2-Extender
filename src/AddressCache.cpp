#include "AddressCache.h"

void* AddressCache::Get(const char* name) {
    auto it = m_AddressMap.find(name);
    if (it != m_AddressMap.end()) {
        return it->second;
    }
    return nullptr;
}

void AddressCache::Set(const char* name, void* offset) {
    m_AddressMap[name] = offset;
}

void AddressCache::Write(std::wstring path) {

}

void AddressCache::Read(std::wstring path) {

}