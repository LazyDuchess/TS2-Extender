#include "framework.h"
#include "Psapi.h"
#include "Addresses.h"
#include "scan.h"

namespace Addresses {

	static char randomUint32Lookup[] = { 0x8B, 0x01, 0xBA, 0x6D, 0x4E, 0xC6, 0x41, 0xF7, 0xE2, 0x05, 0x39, 0x30, 0x00, 0x00 };
	static char randomUint32LookupMask[] = "xxxxxxxxxxxxxx";

	void* RandomUint32Uniform;

	bool Initialize() {
		HMODULE module = GetModuleHandleA(NULL);
		char* modBase = (char*)module;
		HANDLE proc = GetCurrentProcess();
		MODULEINFO modInfo;
		GetModuleInformation(proc, module, &modInfo, sizeof(MODULEINFO));
		int size = modInfo.SizeOfImage;
		RandomUint32Uniform = ScanInternal(randomUint32Lookup, randomUint32LookupMask, modBase, size);
		if (RandomUint32Uniform == nullptr) return false;
		return true;
	}
}