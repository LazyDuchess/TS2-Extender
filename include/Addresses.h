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
	extern void* LuaToNumber;
	extern void* LuaPushNumber;
	extern void* LuaNewTable;
	extern void* LuaPushCClosure;
	extern void* LuaGetTop;
	extern void* LuaGetTable;
	extern void* LuaSetTable;
	extern void* LuaPushBoolean;
	extern void* LuaLUnref;
	extern void* LuaToBoolean;

	extern void* ClothingDialogOnCancel;
	extern void* ClothingDialogOnAttach;
	extern void* ClothingDialogSetState;
	extern void* DressEmployeeDialogOnAttach;
	extern void* ClothingDialogHack1;
	extern void* ClothingDialogHack2;

	extern void* CheatQueryInterface;
	extern void* CheatRelease;
	extern void* CheatDestructor;
	extern void* GetCheatSystem;
	extern void* RegisterTestingCheat;
	extern void* RegisterTSSGCheats;
	extern void* CheatSystem;

	extern void* CalculateOutfitPartVisibility;
	extern void* CalculateBuyPartVisibility;
	extern void* CalculateTryOnPartVisibility;

	extern void* GetNodeTextInputField;

	extern void* TSStringLoad;

	extern void* LoadUIScript;

	extern void* CRZStringFromChar;

	extern void* UIMakeMoneyString;

	extern void* AppendInteractionsForMenu;

	extern void* AddCheatInteraction;

	extern void* TSGlobalsCall;
	extern void* TSGlobals;

	extern void* LAAPointerCheck;

	extern void* cEMVoxModifierModifyEvent;

	extern void* cTSUserToolObjectInit;
	extern void* cTSUserToolObjectShutdown;
	
	extern void* LegacyCalculateUIScale;

	extern void* CASStringLookup;

	extern char* CASLotName;
	extern char* YACASLotName;

	extern void* cTSSGSystemOncePerFrameUpdate;
	extern void* cTSUICASComponentOverlaysOnTick;
	extern void* cTSUICASComponentOverlaysDoMessage;
	extern void* cTSUICASComponentOverlaysActivate;
	// no clue but triggers when switching tabs in CAS UI
	extern void* UnknownUITabChange;
	// triggers when switching tabs in mirror UI
	extern void* UnknownMirrorUITabChange;

	extern void* ScenegraphAddGameVersion;

	extern void* SetupUIForSimCreation;
	extern void* SimEditorUpdateUI;

	// no clue but walking back from cTSUISimEditorBase::SetPersonData got me here, where the zodiac is recalculated after editing personality.
	extern void* CalcZodiacAddress;

	extern void* SimEditorLoadCASComponent;

	extern void* SimEditorValidateSim;
	extern void* SimEditorDoMessage;
	extern void* SimEditorHandleTabWizard;

	bool Initialize();
}