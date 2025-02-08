#include "framework.h"
#include "Psapi.h"
#include "Addresses.h"
#include "scan.h"
#include "Logging.h"

namespace Addresses {

	static char randomUint32Lookup[] = { 0x8B, 0x01, 0xBA, 0x6D, 0x4E, 0xC6, 0x41, 0xF7, 0xE2, 0x05, 0x39, 0x30, 0x00, 0x00 };
	static char randomUint32LookupMask[] = "xxxxxxxxxxxxxx";

	// first 4 bytes are a pointer to ealogo_audio.movie
	static char eaLogoPushLookup[] = { 
		0xE8, 0x2B, 0xBF, 0x01,
		0x8D, 0x4D, 0xBC, 
		0xE8, 0x76, 0x05, 0xB8, 0xFF, 
		0x83, 0x65, 0xD4, 0x00, 
		0xBF, 0xB0, 0x54, 0xBB, 0x01, 
		0x89, 0x7D, 0xB8, 
		0x33, 0xC0,
		0xC7, 0x45 , 0xFC , 0x08, 0x00, 0x00, 0x00 
	};
	static char eaLogoPushLookupMask[] = "????xxxx????xxxxx????xxxxxxxxxxxx";

	// first 4 bytes are a pointer to intro_eng_audio.movie
	static char introEngPushLookup[] = { 
		0xFC, 0x2B, 0xBF, 0x01, 
		0x8D, 0x4D, 0xBC, 
		0xE8, 0x65, 0x03, 0xB8, 0xFF, 
		0x83, 0x65, 0xD4, 0x00, 
		0xBF, 0xB0, 0x54, 0xBB, 0x01, 
		0x89, 0x7D, 0xB8, 
		0x6A, 0x00, 
		0x8D, 0x45, 0xB8, 
		0xC7, 0x45, 0xFC, 0x0C, 0x00, 0x00, 0x00 
	};
	static char introEngPushLookupMask[] = "????xxxx????xxxxx????xxxxxxxxxxxxxxx";

	static char luaUnregisterLookup[] = { 
		0x56, 
		0x8B, 0x74, 0x24, 0x08, 
		0x8B, 0xCE, 
		0xFF, 0x74, 0x24, 0x0C, 
		0x8B, 0x06, 
		0xFF, 0x90, 0xD4, 0x00, 0x00, 0x00, 
		0x8B, 0x06, 
		0x8B, 0xCE 
	};
	static char luaUnregisterLookupMask[] = "xxxxxxxxxxxxxxxxxxxxxxx";

	static char luaPrintStubLookup[] = { 
		0xFF, 0x50, 0x10, 
		0x8B, 0xF8, 
		0x8B, 0xCE, 
		0x8B, 0x06, 
		// Stub from here
		0x68, 0xF4, 0x0B, 0xA0, 0x01, 
		0x68, 0xB0, 0xC6, 0xAF, 0x00, 
		0xFF, 0x90, 0x50, 0x01, 0x00, 0x00, 
		// End
		0x68, 0xEF, 0xD8, 0xFF, 0xFF 
	};
	static char luaPrintStubLookupMask[] = "xxxxxxxxxx????x????xxxxxxxxxxx";

	static char lua5OpenLookup[] = {
		0x53,
		0x55,
		0x56,
		0x57,
		0x8B, 0xF1,
		0xE8, 0xE9, 0x75, 0xD6, 0xFF,
		0x8B, 0xE8,
		0x85, 0xED,
		0x75, 0x0A,
		0x6A, 0x01,
		0xE8, 0x06, 0x76, 0xD6, 0xFF,
		0x83, 0xC4, 0x04
	};
	static char lua50OpenLookupMask[] = "xxxxxxx????xxxxxxxxx????xxx";

	static char registerLuaCommandsLookup[] = {
		0x56,
		0x8B, 0x74, 0x24, 0x08,
		0x32, 0xC0,
		0x85, 0xF6,
		0x0F, 0x84, 0xCE, 0x01, 0x00, 0x00,
		0x8B, 0x06,
		0x8B, 0xCE,
		0x68, 0x60, 0x3F, 0x08, 0x01,
		0x68, 0xA0, 0x18, 0x83, 0x00,
		0xFF, 0x90, 0x50, 0x01, 0x00, 0x00
	};
	static char registerLuaCommandsLookupMask[] = "xxxxxxxxxx?????xxxxx????x????xxxxxx";

	static char luaPushStringLookup[] = {
		0x55,
		0x8B, 0x6C, 0x24, 0x0C,
		0x85, 0xED,
		0x75, 0x0F,
		0x8B, 0x4C, 0x24, 0x08,
		0x8B, 0x41, 0x08,
		0x89, 0x28,
		0x83, 0x41, 0x08, 0x10,
		0x5D,
		0xC3
	};
	static char luaPushStringLookupMask[] = "xxxxxxxxxxxxxxxxxxxxxxxx";

	static char cheatQueryInterfaceLookup[] = {
		0x8B, 0x44, 0x24, 0x08,
		0x85, 0xC0,
		0x75, 0x04,
		0x32, 0xC0,
		0xEB, 0x23,
		0x83, 0x7C, 0x24, 0x04, 0x01,
		0x74, 0x13,
		0x81, 0x7C, 0x24, 0x04, 0xC7, 0x39, 0xDA, 0xAA
	};
	static char cheatQueryInterfaceLookupMask[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxx";

	static char cheatReleaseLookup[] = {
		0x8B, 0x41, 0x08,
		0x83, 0xE8, 0x01,
		0x89, 0x41, 0x08,
		0x75, 0x09,
		0x8B, 0x01,
		0xFF, 0x50, 0x24,
		0x33, 0xC0,
		0xC3
	};
	static char cheatReleaseLookupMask[] = "xxxxxxxxxxxxxxxxxxx";

	static char cheatDestructorLookup[] = {
		0xF6, 0x44, 0x24, 0x04, 0x01,
		0x56,
		0x8B, 0xF1,
		0xC7, 0x06, 0x9C, 0x1A, 0x05, 0x01,
		0x74, 0x0B,
		0x6A, 0x0C,
		0x56
	};
	static char cheatDestructorLookupMask[] = "xxxxxxxxxx????xxxxx";

	void* RandomUint32Uniform;
	void* EALogoPush;
	void* IntroPush;
	void* LuaUnregister;
	void* LuaPrintStub;
	void* GZLua5Open;
	void* RegisterLuaCommands;
	void* LuaPushString;

	void* CheatQueryInterface;
	void* CheatRelease;
	void* CheatDestructor;

	static bool ScanBaseAddresses(char* modBase, int size) {
		RandomUint32Uniform = ScanInternal(randomUint32Lookup, randomUint32LookupMask, modBase, size);
		if (RandomUint32Uniform == nullptr) return false;
		EALogoPush = ScanInternal(eaLogoPushLookup, eaLogoPushLookupMask, modBase, size);
		if (EALogoPush == nullptr) return false;
		IntroPush = ScanInternal(introEngPushLookup, introEngPushLookupMask, modBase, size);
		if (IntroPush == nullptr) return false;
		LuaUnregister = ScanInternal(luaUnregisterLookup, luaUnregisterLookupMask, modBase, size);
		if (LuaUnregister == nullptr) return false;
		LuaPrintStub = ScanInternal(luaPrintStubLookup, luaPrintStubLookupMask, modBase, size);
		if (LuaPrintStub == nullptr) return false;
		LuaPrintStub = (void*)((DWORD)LuaPrintStub + 9);
		GZLua5Open = ScanInternal(lua5OpenLookup, lua50OpenLookupMask, modBase, size);
		if (GZLua5Open == nullptr) return false;
		RegisterLuaCommands = ScanInternal(registerLuaCommandsLookup, registerLuaCommandsLookupMask, modBase, size);
		if (RegisterLuaCommands == nullptr) return false;
		LuaPushString = ScanInternal(luaPushStringLookup, luaPushStringLookupMask, modBase, size);
		if (LuaPushString == nullptr) return false;
		return true;
	}

	static bool ScanCheatAddresses(char* modBase, int size) {
		CheatQueryInterface = ScanInternal(cheatQueryInterfaceLookup, cheatQueryInterfaceLookupMask, modBase, size);
		if (CheatQueryInterface == nullptr) return false;
		CheatRelease = ScanInternal(cheatReleaseLookup, cheatReleaseLookupMask, modBase, size);
		if (CheatRelease == nullptr) return false;
		CheatDestructor = ScanInternal(cheatDestructorLookup, cheatDestructorLookupMask, modBase, size);
		if (CheatDestructor == nullptr) return false;
		return true;
	}

	bool Initialize() {
		HMODULE module = GetModuleHandleA(NULL);
		char* modBase = (char*)module;
		HANDLE proc = GetCurrentProcess();
		MODULEINFO modInfo;
		GetModuleInformation(proc, module, &modInfo, sizeof(MODULEINFO));
		int size = modInfo.SizeOfImage;
		if (!ScanBaseAddresses(modBase, size)) return false;
		if (!ScanCheatAddresses(modBase, size)) return false;
		return true;
	}
}