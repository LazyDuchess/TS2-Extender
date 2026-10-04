#pragma once

class cIGZWin;

class cGZWinMgrBase {
public:

	bool IsModal();
	cIGZWin* GZGetFocus();
};