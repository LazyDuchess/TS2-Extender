#include "framework.h"
#include "Psapi.h"
#include "Addresses.h"
#include "scan.h"

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
		0xC7, 0x45 , 0xFC , 0x08, 0x00, 0x00, 0x00 };
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
		0xC7, 0x45, 0xFC, 0x0C, 0x00, 0x00, 0x00 };
	static char introEngPushLookupMask[] = "????xxx?xxxxxxxxx????xxxxxxxxxxxxxxx";

	void* RandomUint32Uniform;
	void* EALogoPush;
	void* IntroPush;

	bool Initialize() {
		HMODULE module = GetModuleHandleA(NULL);
		char* modBase = (char*)module;
		HANDLE proc = GetCurrentProcess();
		MODULEINFO modInfo;
		GetModuleInformation(proc, module, &modInfo, sizeof(MODULEINFO));
		int size = modInfo.SizeOfImage;
		RandomUint32Uniform = ScanInternal(randomUint32Lookup, randomUint32LookupMask, modBase, size);
		if (RandomUint32Uniform == nullptr) return false;
		EALogoPush = ScanInternal(eaLogoPushLookup, eaLogoPushLookupMask, modBase, size);
		if (EALogoPush == nullptr) return false;
		IntroPush = ScanInternal(introEngPushLookup, introEngPushLookupMask, modBase, size);
		if (IntroPush == nullptr) return false;
		return true;
	}
}