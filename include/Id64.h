#pragma once
#include <cstdint>

class Id64 {
public:
    Id64() {

    }
    Id64(uint64_t value) {
        m_Value = value;
    }
    Id64(uint32_t index, uint32_t generation) {
        m_Value = (uint64_t(generation) << 32) | uint64_t(index);
    }
    uint32_t Index() const {
        return uint32_t(m_Value);
    }

    uint32_t Generation() const {
        return uint32_t(m_Value >> 32);
    }

    bool IsValid() const {
        return m_Value != 0;
    }

    uint64_t m_Value = 0;
};