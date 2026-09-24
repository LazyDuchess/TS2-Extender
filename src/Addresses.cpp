#include "framework.h"
#include "Psapi.h"
#include "Addresses.h"
#include "scan.h"
#include "Logging.h"
#include "ts2/cUserInput.h"

#define ADDRESS_REQUIRE(name, lookup) \
Log("Scanning for %s...\n", #name);\
name = ScanInternal(lookup, lookup##Mask, modBase, size);\
if (name == nullptr) {\
	Log("FATAL: Failed to find address for %s!\n", #name);\
	return false;\
}\
else\
{\
	Log("Found %s at %p\n", #name, name);\
}\

#define ADDRESS(name, lookup) \
Log("Scanning for %s...\n", #name);\
name = ScanInternal(lookup, lookup##Mask, modBase, size);\
if (name == nullptr) {\
	Log("Failed to find address for %s!\n", #name);\
}\
else\
{\
	Log("Found %s at %p\n", #name, name);\
}\

namespace Addresses {

	#include "Lookups.h"

	void* RandomUint32Uniform;
	void* EALogoPush;
	void* IntroPush;
	void* LuaUnregister;
	void* LuaPrintStub;
	void* GZLua5Open;
	void* RegisterLuaCommands;

	void* LuaPushString;
	void* LuaRawGetI;
	void* LuaSetTop;
	void* LuaPCall;
	void* LuaPushValue;
	void* LuaToString;
	void* LuaLRef;
	void* LuaToNumber;
	void* LuaPushNumber;
	void* LuaNewTable;
	void* LuaPushCClosure;
	void* LuaGetTop;
	void* LuaGetTable;
	void* LuaSetTable;
	void* LuaPushBoolean;
	void* LuaLUnref;
	void* LuaToBoolean;

	void* ClothingDialogOnCancel;
	void* ClothingDialogOnAttach;
	void* ClothingDialogSetState;
	void* DressEmployeeDialogOnAttach;
	void* ClothingDialogHack1;
	void* ClothingDialogHack2;

	void* CheatQueryInterface;
	void* CheatRelease;
	void* CheatDestructor;
	void* GetCheatSystem;
	void* RegisterTestingCheat;
	void* RegisterTSSGCheats;

	void* CalculateOutfitPartVisibility;
	void* CalculateBuyPartVisibility;
	void* CalculateTryOnPartVisibility;

	void* GetNodeTextInputField;

	void* TSStringLoad;

	void* LoadUIScript;

	void* CRZStringFromChar;

	void* UIMakeMoneyString;

	void* AppendInteractionsForMenu;

	void* AddCheatInteraction;

	void* TSGlobalsCall;
	void* TSGlobals;

	void* LAAPointerCheck;

	void* cEMVoxModifierModifyEvent;

	void* cTSUserToolObjectInit;
	void* cTSUserToolObjectShutdown;

	void* LegacyCalculateUIScale;

	void* CASStringLookup;

	char* CASLotName;
	char* YACASLotName;

	void* cTSSGSystemOncePerFrameUpdate;
	void* cTSUICASComponentOverlaysOnTick;
	void* cTSUICASComponentOverlaysDoMessage;
	void* cTSUICASComponentOverlaysActivate;

	void* UnknownUITabChange;
	void* UnknownMirrorUITabChange;

	void* ScenegraphAddGameVersion;

	void* SetupUIForSimCreation;
	void* SimEditorUpdateUI;

	void* CalcZodiacAddress;

	void* SimEditorLoadCASComponent;

	void* SimEditorValidateSim;
	void* SimEditorDoMessage;
	void* SimEditorHandleTabWizard;

	static bool ScanBaseAddresses(char* modBase, int size) {
		ADDRESS(RandomUint32Uniform, randomUint32Lookup);
		ADDRESS(EALogoPush, eaLogoPushLookup);
		ADDRESS(IntroPush, introEngPushLookup);
		ADDRESS(LuaUnregister, luaUnregisterLookup);
		ADDRESS(LuaPrintStub, luaPrintStubLookup);
#if TS2_LC
		if (ADDRESS_VALID(LuaPrintStub))
			LuaPrintStub = (void*)((DWORD)LuaPrintStub + 9);
#endif
		ADDRESS(GZLua5Open, lua5OpenLookup);
		ADDRESS(RegisterLuaCommands, registerLuaCommandsLookup);
		ADDRESS(LuaPushString, luaPushStringLookup);
		ADDRESS(LuaRawGetI, luaRawGetILookup);
		ADDRESS(LuaSetTop, luaSetTopLookup);
		ADDRESS(LuaPCall, luaPCallLookup);
		ADDRESS(LuaPushValue, luaPushValueLookup);
		ADDRESS(LuaToString, luaToStringLookup);
		ADDRESS(LuaLRef, luaLRefLookup);
		ADDRESS(LuaToNumber, luaToNumberLookup);
		ADDRESS(LuaPushNumber, luaPushNumberLookup);
		ADDRESS(LuaNewTable, luaNewTableLookup);
		ADDRESS(LuaPushCClosure, luaPushCClosureLookup);
		ADDRESS(LuaGetTop, luaGetTopLookup);
		ADDRESS(LuaGetTable, luaGetTableLookup);
		ADDRESS(LuaSetTable, luaSetTableLookup);
		ADDRESS(LuaPushBoolean, luaPushBooleanLookup);
		ADDRESS(LuaLUnref, luaLUnrefLookup);
		ADDRESS(LuaToBoolean, luaToBooleanLookup);
		ADDRESS(ClothingDialogOnCancel, clothingDialogOnCancelLookup);
		ADDRESS(ClothingDialogOnAttach, clothingDialogOnAttachLookup);
		ADDRESS(ClothingDialogSetState, clothingDialogSetStateLookup);
		ADDRESS(DressEmployeeDialogOnAttach, dressEmployeeDialogOnAttachLookup);
		ADDRESS(ClothingDialogHack1, clothingDialogHack1Lookup);
		ADDRESS(ClothingDialogHack2, clothingDialogHack2Lookup);
		ADDRESS(CalculateOutfitPartVisibility, calculateOutfitPartVisibilityLookup);
		ADDRESS(CalculateBuyPartVisibility, calculateBuyPartVisibilityLookup);
		ADDRESS(CalculateTryOnPartVisibility, calculateTryOnPartVisibilityLookup);
		ADDRESS(GetNodeTextInputField, getNodeTextInputFieldLookup);
		if (ADDRESS_VALID(GetNodeTextInputField))
			cUserInput::m_GlobalUserInputPtr = (cUserInput**)*((cUserInput***)GetNodeTextInputField);
		ADDRESS(TSStringLoad, tsStringLoadLookup);
		ADDRESS(LoadUIScript, loadUiScriptLookup);
		ADDRESS(CRZStringFromChar, crzstringFromCharLookup);
		ADDRESS(UIMakeMoneyString, uiMakeMoneyStringLookup);
		ADDRESS(AppendInteractionsForMenu, appendInteractionsForMenuLookup);
		ADDRESS(AddCheatInteraction, addCheatInteractionLookup);
		ADDRESS(TSGlobalsCall, tsGlobalsCallLookup);
		if (ADDRESS_VALID(TSGlobalsCall)) {
			DWORD relativeCall = *(DWORD*)TSGlobalsCall;
			TSGlobals = (void*)((DWORD)TSGlobalsCall + relativeCall + 4);
		}
		ADDRESS(LAAPointerCheck, laaPointerCheckLookup);
		ADDRESS(cEMVoxModifierModifyEvent, voxModifierModifyEventLookup);
		ADDRESS(cTSUserToolObjectInit, cTSUserToolObjectInitLookup);
		ADDRESS(cTSUserToolObjectShutdown, cTSUserToolObjectShutdownLookup);
		ADDRESS(LegacyCalculateUIScale, LegacyCalculateUIScaleLookup);
		ADDRESS(CASStringLookup, CASStringLookupLookup);
		ADDRESS(cTSSGSystemOncePerFrameUpdate, cTSSGSystemOncePerFrameUpdateLookup);
		ADDRESS(cTSUICASComponentOverlaysOnTick, cTSUICASComponentOverlaysOnTickLookup);
		ADDRESS(cTSUICASComponentOverlaysDoMessage, cTSUICASComponentOverlaysDoMessageLookup);
		ADDRESS(cTSUICASComponentOverlaysActivate, cTSUICASComponentOverlaysActivateLookup);
		ADDRESS(UnknownUITabChange, UnknownUITabChangeLookup);
		ADDRESS(UnknownMirrorUITabChange, UnknownMirrorUITabChangeLookup);
		ADDRESS(ScenegraphAddGameVersion, ScenegraphAddGameVersionLookup);
		ADDRESS(SetupUIForSimCreation, SetupUIForSimCreationLookup);
		ADDRESS(SimEditorUpdateUI, SimEditorUpdateUILookup);
		ADDRESS(CalcZodiacAddress, CalcZodiacAddressLookup);
		ADDRESS(SimEditorLoadCASComponent, SimEditorLoadCASComponentLookup);
		ADDRESS(SimEditorValidateSim, SimEditorValidateSimLookup);
		ADDRESS(SimEditorDoMessage, SimEditorDoMessageLookup);
		ADDRESS(SimEditorHandleTabWizard, SimEditorHandleTabWizardLookup);

		if (ADDRESS_VALID(CASStringLookup)) {
			CASLotName = *(char**)((DWORD)CASStringLookup + 0x6);
			YACASLotName = *(char**)((DWORD)CASStringLookup + 0x18);
		}
		return true;
	}

	static bool ScanCheatAddresses(char* modBase, int size) {
		ADDRESS(CheatQueryInterface, cheatQueryInterfaceLookup);
		ADDRESS(CheatRelease, cheatReleaseLookup);
		ADDRESS(CheatDestructor, cheatDestructorLookup);
		ADDRESS(GetCheatSystem, getCheatSystemLookup);
		ADDRESS(RegisterTestingCheat, registerTestingCheatLookup);
		ADDRESS(RegisterTSSGCheats, registerTSSGCheatsLookup);
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