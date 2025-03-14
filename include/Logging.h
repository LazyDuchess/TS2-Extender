#pragma once
#include <iostream>
#include "config.h"
#include <stdarg.h>

inline void Log(const char* format, ...) {
#if !FORCE_CONSOLE
    if (Config::Console) {
#endif
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
#if !FORCE_CONSOLE
    }
#endif
}