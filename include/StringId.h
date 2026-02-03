#pragma once
#include <cstdint>
#include <functional>

class StringId {
public:
	uint64_t m_Value = 0;
	StringId() {};
	StringId(uint16_t index, uint16_t instance, uint32_t group) {
		m_Value = uint64_t(index) | (uint64_t(instance) << 16) | (uint64_t(group) << 32);
	}
	uint16_t Index() const {
		return uint16_t(m_Value);
	}

	uint16_t Instance() const {
		return uint16_t(m_Value >> 16);
	}

	uint32_t Group() const {
		return uint32_t(m_Value >> 32);
	}

	bool operator==(const StringId& other) const noexcept {
		return m_Value == other.m_Value;
	}
};

namespace std {
	template<>
	struct hash<StringId> {
		size_t operator()(const StringId& k) const noexcept {
			return std::hash<uint64_t>{}(k.m_Value);
		}
	};
}