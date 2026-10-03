#include <Windows.h>

namespace SplashWindow {
	extern HMODULE gModule;
	void Create(const char* splashDirectory, float verticalCoverage);
	void SignalClose();
}