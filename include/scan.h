#pragma once
#include <Windows.h>

void WriteToMemory(DWORD addressToWrite, void* valueToWrite, int byteNum);
char* ScanBasic(char* pattern, char* mask, char* begin, int size);
char* ScanInternal(char* pattern, char* mask, char* begin, int size);