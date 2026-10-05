#pragma once
#include "ts2/ts2.h"

class cIGZUnknown {
public:
    virtual bool QueryInterface(iid_t id, cIGZUnknown** out);
    virtual int AddRef();
    virtual int Release();
};