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

	bool Initialize();
}