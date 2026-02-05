#include "LuaCallback.h"
#include <algorithm>

LuaCallback::LuaCallback(int luaCall, lua_State* luaState, int priority) {
	m_LuaCall = luaCall;
	m_luaState = luaState;
	m_Priority = priority;
}

Id64 LuaDelegate::AddCallback(LuaCallback callback) {
	Id64 id = m_IdManager.Acquire();
	callback.m_Id = id;
	m_Callbacks.push_back(callback);
	Sort();
	return id;
}

void LuaDelegate::RemoveCallback(Id64 callbackId) {
	for (auto it = m_Callbacks.begin(); it != m_Callbacks.end(); ) {
		auto callback = it;
		if (callback->m_Id.m_Value == callbackId.m_Value)
		{
			m_IdManager.Release(callbackId);
			it = m_Callbacks.erase(it);
			return;
		}

		++it;
	}
}

void LuaDelegate::Sort() {
	std::sort(m_Callbacks.begin(), m_Callbacks.end(),
		[](const LuaCallback& a, const LuaCallback& b)
		{
			return a.m_Priority < b.m_Priority;
		});
}