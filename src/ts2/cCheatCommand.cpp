#include "ts2/cCheatCommand.h"
#include "Addresses.h"

namespace TS2 {
	void* cCheatCommand::QueryInterface(int typeId, void** unknown) {
		using QueryInterfaceFunc = void*(__thiscall*)(cCheatCommand* me, int typeId, void** unknown);
		QueryInterfaceFunc func = reinterpret_cast<QueryInterfaceFunc>(Addresses::CheatQueryInterface);
		return func(this, typeId, unknown);
	}
	void cCheatCommand::AddRef() {
		ReferenceCount++;
	}
	int cCheatCommand::Release() {
		using ReleaseFunc = int (__thiscall*)(cCheatCommand* me);
		ReleaseFunc func = reinterpret_cast<ReleaseFunc>(Addresses::CheatRelease);
		return func(this);
	}
	void cCheatCommand::OnRegister(void* parser, void* unk) {
		Parser = parser;
	}
	const char* cCheatCommand::Name() {
		return "";
	}
	const char* cCheatCommand::DescriptionForHelp(int helpType) {
		return Description();
	}
	int cCheatCommand::HandlesAllArguments() {
		return 0;
	}
	int cCheatCommand::GetConditionalLevelAdjustment() {
		return 0;
	}
	void* cCheatCommand::Destructor(bool freeMemory) {
		using DestructorFunc = void*(__thiscall*)(cCheatCommand* me, bool freeMemory);
		DestructorFunc func = reinterpret_cast<DestructorFunc>(Addresses::CheatDestructor);
		return func(this, freeMemory);
	}
	const char* cCheatCommand::Description() {
		return "";
	}
	void* cCheatCommand::CheatSystemParser() {
		return Parser;
	}
}