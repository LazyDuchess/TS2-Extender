#pragma once
#include <iostream>
#include "config.h"
#include <stdarg.h>

inline void Log(const char* format, ...) {
    if (Config::Console) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}