#include "AddressCache.h"
#include <iostream>
#include <fstream>

#define ADDRESSCACHE_VERSION 0

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
    std::ofstream outfile(path, std::ios::out | std::ios::binary);

    if (!outfile) return;

    int version = ADDRESSCACHE_VERSION;
    int count = m_AddressMap.size();
    outfile.write(reinterpret_cast<const char*>(&version), sizeof(version));
    outfile.write(reinterpret_cast<const char*>(&count), sizeof(count));

    for (const auto& [key, address] : m_AddressMap) {
        outfile.write(reinterpret_cast<const char*>(key.c_str()), strlen(key.c_str()) + 1);
        outfile.write(reinterpret_cast<const char*>(&address), sizeof(address));
    }

    outfile.close();
}

void AddressCache::Read(std::wstring path) {
    std::ifstream infile(path, std::ios::binary);
    if (!infile) return;

    int version;
    int count;

    if (!infile.read(reinterpret_cast<char*>(&version), sizeof(version))) return;

    if (version > ADDRESSCACHE_VERSION) return;

    if (!infile.read(reinterpret_cast<char*>(&count), sizeof(count))) return;

    if (count < 0) return;

    for (int i = 0; i < count; i++) {
        std::string key;
        void* addr;
        if (!std::getline(infile, key, '\0')) return;
        if (!infile.read(reinterpret_cast<char*>(&addr), sizeof(addr))) return;
        Set(key.c_str(), addr);
    }
    infile.close();
}