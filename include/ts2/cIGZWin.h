#pragma once
#define ENUM_ALL 0x22ba0121
#define WINFLAG_ENABLED 0x2
#define WINFLAG_VISIBLE 0x1

class cIGZWin;

typedef int winflag_t;
typedef int enumQuery_t;
typedef bool(__cdecl* FNENUMCHILDREN)(cIGZWin* parent, int id, cIGZWin* current, void* userdata);

class cIGZWin {
public:
	bool ChildDeleteAll();
	cIGZWin* GetChildWindowFromID(int id);
	cIGZWin* GetChildWindowFromIDRecursive(int id);
	int SetFlag(winflag_t flag, bool value);
	int GetChildCount();
	bool EnumChildren(enumQuery_t query, FNENUMCHILDREN callback, void* userdata);
	int GetID();
};