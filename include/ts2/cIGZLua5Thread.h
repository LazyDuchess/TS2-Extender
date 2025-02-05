#pragma once
#include "lua.h"

namespace TS2 {
	class cIGZLua5Thread {
	public:
		void Register(LUAFUNCTION* pFunc, const char* funcName) {
			int vTableAddr = *(int*)this;
			using RegisterFunc = void(__thiscall*)(cIGZLua5Thread* me, LUAFUNCTION* pFunc, const char*);
			RegisterFunc func = reinterpret_cast<RegisterFunc>(*(int*)(vTableAddr + 0x150));
			func(this, pFunc, funcName);
		}
	};
}