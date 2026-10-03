#include <Windows.h>

namespace SplashWindow {
	extern HMODULE gModule;
	extern HWND gSplashWindow;
	void Create(const char* splashDirectory, const wchar_t* baseDirectory, float verticalCoverage);
	void SignalClose();
}