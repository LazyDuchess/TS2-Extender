#include "scan.h"
#include <Windows.h>

void WriteToMemory(DWORD addressToWrite, void* valueToWrite, int byteNum)
{
    //used to change our file access type, stores the old
    //access type and restores it after memory is written
    unsigned long OldProtection;
    //give that address read and write permissions and store the old permissions at oldProtection
    VirtualProtect((LPVOID)(addressToWrite), byteNum, PAGE_EXECUTE_READWRITE, &OldProtection);

    //write the memory into the program and overwrite previous value
    memcpy((LPVOID)addressToWrite, valueToWrite, byteNum);

    //reset the permissions of the address back to oldProtection after writting memory
    VirtualProtect((LPVOID)(addressToWrite), byteNum, OldProtection, NULL);
}

char* ScanBasic(char* pattern, char* mask, char* begin, int size)
{
    int patternLen = strlen(mask);

    for (int i = 0; i < size; i++)
    {
        bool found = true;
        for (int j = 0; j < patternLen; j++)
        {
            if (mask[j] != '?' && pattern[j] != *(char*)((intptr_t)begin + i + j))
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            return (begin + i);
        }
    }
    return nullptr;
}

char* ScanInternal(char* pattern, char* mask, char* begin, int size)
{
    char* match{ nullptr };
    MEMORY_BASIC_INFORMATION mbi{};

    for (char* curr = begin; curr < begin + size; curr += mbi.RegionSize)
    {
        if (!VirtualQuery(curr, &mbi, sizeof(mbi)) || mbi.State != MEM_COMMIT || mbi.Protect == PAGE_NOACCESS) continue;

        match = ScanBasic(pattern, mask, curr, mbi.RegionSize);

        if (match != nullptr)
        {
            break;
        }
    }
    return match;
}