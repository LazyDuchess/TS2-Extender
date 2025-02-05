#pragma once
#include "lua.h"

namespace TS2 {
	class cIGZLua5Thread {
	private:
		char[0x150] pad;
	public:
		virtual void Register(LUAFUNCTION* pFunc, char* funcName) = 0;
	};
}