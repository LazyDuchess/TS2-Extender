#pragma once
#include "Id64.h"
#include <vector>
#include <cstdint>

class Slot {
public:
	uint32_t m_Generation = 1;
	bool m_Alive = false;
};

class IdManager {
public:
	Id64 Acquire();
	void Release(Id64 id);

private:
	std::vector<Slot> m_Slots;
	std::vector<uint32_t> m_FreeIndices;
};