#pragma once

namespace Addresses {
	extern void* RandomUint32Uniform;
	extern void* EALogoPush;
	extern void* IntroPush;
	extern void* LuaUnregister;
	extern void* LuaPrintStub;
	extern void* GZLua5Open;
	extern void* RegisterLuaCommands;
	extern void* LuaPushString;
	extern void* LuaRawGetI;
	extern void* LuaSetTop;
	extern void* LuaPCall;
	extern void* LuaPushValue;
	extern void* LuaToString;
	extern void* LuaLRef;

	extern void* CheatQueryInterface;
	extern void* CheatRelease;
	extern void* CheatDestructor;
	extern void* GetCheatSystem;
	extern void* RegisterTestingCheat;
	extern void* RegisterTSSGCheats;

	bool Initialize();
}