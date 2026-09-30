#pragma once
#include "ts2.h"

struct HWND__;
typedef HWND__* HWND;

class cGZFramework {
public:
    bool QueryInterface(iid_t id, void** out);
    int AddRef();
    int Release();
};

// More guesses i love guesses!
class cIGZApp {
public:
    int Release();
    HWND GetMainHWND();
};

cGZFramework* RZGetFramework();