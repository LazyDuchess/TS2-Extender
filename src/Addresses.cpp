#include "framework.h"
#include "Psapi.h"
#include "Addresses.h"
#include "scan.h"
#include "Logging.h"
#include "ts2/cUserInput.h"
#include <memory>
#include "AddressCache.h"
#include <filesystem>

#define ADDRESS(name, lookup) \
name = sAddressCache->Get(#name);\
if (name != nullptr)\
{\
	name = (void*)((DWORD)name + (DWORD)modBase);\
	if (CheckPattern(lookup, lookup##Mask, (char*)name))\
	{\
		Log("Loaded %s from cache at %p (Sims2.exe+%p)\n", #name, name, (void*)((DWORD)name - (DWORD)modBase));\
	}\
	else\
	{\
		name = nullptr;\
	}\
}\
if (name == nullptr)\
{\
	Log("Scanning for %s...\n", #name);\
	name = ScanInternal(lookup, lookup##Mask, modBase, size);\
	if (name == nullptr) {\
		Log("Failed to find address for %s!\n", #name);\
	}\
	else\
	{\
		Log("Found %s at %p (Sims2.exe+%p)\n", #name, name, (void*)((DWORD)name - (DWORD)modBase));\
		sAddressCache->Set(#name,(void*)((DWORD)name - (DWORD)modBase));\
	}\
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

	void* PoolManagerUpdate;
	void* ShadowManagerCtor;
	void* ShadowUpdateSettings;
	void* ShadowIsOutside;

	void* EffectsManagerCreateVisualEffect;

	void* RTAspectRatioCheck;

	void* CheatParserExecuteCommand;

	void* GetMaterialParser;

	void* PostLoadLot;

	void* NhoodEntered;

	void* LotImposterManagerCtor;

	void* DeviceIsFullscreen;

	void* DeviceSetup;

	void* OptionsFillScreenSizeListBox;

	void* SetWindowEnabled;

	void* GetFramework;

	void* CanvasShow;

	void* DesignOnButtonDown;

	void* Sims1CameraHandleRequest;

	void* CameraMiddleClickMouseEvent;

	void* Sims1CameraUpdate;

	void* CameraMiddleClickCancelDrag;

	void* RealtimeThumbnailGender;

	void* OceanReflectionCheck;

	void* ToggleFullscreen;

	void* RequestAnimationError;

	void* Iterations;

	int* MaxIterations;

	void* LoadGroupMap;

	void* SaveGroupMap;

	void* WinManager;

	static std::unique_ptr<AddressCache> sAddressCache = std::make_unique<AddressCache>();

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
#if TS2_LC
		ADDRESS(LegacyCalculateUIScale, LegacyCalculateUIScaleLookup);
#else
		LegacyCalculateUIScale = nullptr;
#endif
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
		ADDRESS(PoolManagerUpdate, PoolManagerUpdateLookup);
		ADDRESS(ShadowManagerCtor, ShadowManagerCtorLookup);
		ADDRESS(ShadowUpdateSettings, ShadowUpdateSettingsLookup);
		ADDRESS(EffectsManagerCreateVisualEffect, EffectsManagerCreateVisualEffectLookup);
		ADDRESS(RTAspectRatioCheck, RTAspectRatioCheckLookup);
		ADDRESS(CheatParserExecuteCommand, CheatParserExecuteCommandLookup);
		ADDRESS(GetMaterialParser, GetMaterialParserLookup);
		ADDRESS(PostLoadLot, PostLoadLotLookup);
		ADDRESS(NhoodEntered, NhoodEnteredLookup);
		ADDRESS(LotImposterManagerCtor, LotImposterManagerCtorLookup);
#if TS2_UC
		ADDRESS(DeviceIsFullscreen, DeviceIsFullscreenLookup);
		ADDRESS(DeviceSetup, DeviceSetupLookup);
		ADDRESS(OptionsFillScreenSizeListBox, OptionsFillScreenSizeListBoxLookup);
		ADDRESS(ToggleFullscreen, ToggleFullscreenLookup);
#endif
		ADDRESS(CanvasShow, CanvasShowLookup);
		ADDRESS(SetWindowEnabled, SetWindowEnabledLookup);
		ADDRESS(GetFramework, GetFrameworkLookup);
		ADDRESS(DesignOnButtonDown, DesignOnButtonDownLookup);
		ADDRESS(Sims1CameraHandleRequest, Sims1CameraHandleRequestLookup);
		ADDRESS(CameraMiddleClickMouseEvent, CameraMiddleClickMouseEventLookup);
		ADDRESS(Sims1CameraUpdate, Sims1CameraUpdateLookup);
		ADDRESS(RealtimeThumbnailGender, RealtimeThumbnailGenderLookup);
		ADDRESS(OceanReflectionCheck, OceanReflectionCheckLookup);
		ADDRESS(RequestAnimationError, RequestAnimationErrorLookup);
		ADDRESS(Iterations, IterationsLookup);
#if TS2_UC
		ADDRESS(LoadGroupMap, LoadGroupMapLookup);
#endif
		ADDRESS(SaveGroupMap, SaveGroupMapLookup);
		ADDRESS(WinManager, WinManagerLookup);
		if (ADDRESS_VALID(WinManager)) {
			DWORD relativeCall = *(DWORD*)WinManager;
			WinManager = (void*)((DWORD)WinManager + relativeCall + 4);
		}

		if (ADDRESS_VALID(Iterations)) {
			MaxIterations = *(int**)Iterations;
		}

		if (ADDRESS_VALID(CameraMiddleClickMouseEvent))
		{
#if TS2_LC
			DWORD callAddr = (DWORD)CameraMiddleClickMouseEvent + 0x171;
#else
			DWORD callAddr = (DWORD)CameraMiddleClickMouseEvent + 0x14A;
#endif
			DWORD relativeCall = *(DWORD*)callAddr;
			CameraMiddleClickCancelDrag = (void*)((DWORD)callAddr + relativeCall + 4);
		}

		if (ADDRESS_VALID(GetFramework)) {
			DWORD relativeCall = *(DWORD*)GetFramework;
			GetFramework = (void*)((DWORD)GetFramework + relativeCall + 4);
		}

		if (ADDRESS_VALID(GetMaterialParser)) {
			DWORD relativeCall = *(DWORD*)GetMaterialParser;
			GetMaterialParser = (void*)((DWORD)GetMaterialParser + relativeCall + 4);
		}

		if (ADDRESS_VALID(ShadowUpdateSettings)) {
#if TS2_LC
			DWORD relativeCall = *(DWORD*)((DWORD)ShadowUpdateSettings + 0x3A);
			ShadowIsOutside = (void*)((DWORD)((DWORD)ShadowUpdateSettings + 0x3A) + relativeCall + 4);
#else
			DWORD relativeCall = *(DWORD*)((DWORD)ShadowUpdateSettings + 0x2E);
			ShadowIsOutside = (void*)((DWORD)((DWORD)ShadowUpdateSettings + 0x2E) + relativeCall + 4);
#endif
		}

		if (ADDRESS_VALID(CASStringLookup)) {
#if TS2_LC
			CASLotName = *(char**)((DWORD)CASStringLookup + 0x6);
			YACASLotName = *(char**)((DWORD)CASStringLookup + 0x18);
#else
			CASLotName = *(char**)((DWORD)CASStringLookup + 0x6);
			YACASLotName = *(char**)((DWORD)CASStringLookup + 0x1A);
#endif
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

	bool Initialize(std::wstring userDir) {

		std::filesystem::path userPath = std::filesystem::path(userDir);
		std::filesystem::create_directories(userPath);

		std::wstring userCachePath = userPath.wstring() + L"\\ts2e_cache.bin";

		sAddressCache->Read(userCachePath);

		HMODULE module = GetModuleHandleA(NULL);
		char* modBase = (char*)module;
		HANDLE proc = GetCurrentProcess();
		MODULEINFO modInfo;
		GetModuleInformation(proc, module, &modInfo, sizeof(MODULEINFO));
		int size = modInfo.SizeOfImage;
		bool result = true;
		if (!ScanBaseAddresses(modBase, size))
			result = false;
		if (!ScanCheatAddresses(modBase, size))
			result = false;

		if (result) {
			sAddressCache->Write(userCachePath);
		}

		sAddressCache.reset();
		return result;
	}
}