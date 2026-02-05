#include "IdManager.h"

Id64 IdManager::Acquire() {
	uint32_t index;
	if (!m_FreeIndices.empty()) {
		index = m_FreeIndices.back();
		m_FreeIndices.pop_back();
	}
	else
	{
		index = m_Slots.size();
		m_Slots.emplace_back();
	}

	m_Slots[index].m_Alive = true;
	return Id64(index, m_Slots[index].m_Generation);
}

void IdManager::Release(Id64 id) {
	Slot& slot = m_Slots[id.Index()];
	slot.m_Alive = false;
	slot.m_Generation++;
	if (slot.m_Generation <= 0)
		slot.m_Generation = 1;
	m_FreeIndices.push_back(id.Index());
}