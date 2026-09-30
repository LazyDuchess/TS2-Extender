#pragma once
#include <string>

namespace Config {
	void Load(std::string& basePath);

	extern bool Console;
	extern bool SkipIntro;
	extern bool Borderless;
	extern bool FixRNG;
	extern bool FixOFBUniform;
	extern bool FixPinkFlashing;
	extern bool FixMakeupLag;
	extern bool FixPoolShadows;
	extern bool FixOutdoorShadows;
	extern bool FixSun;
	extern bool ExtendedLua;
	extern bool Separates4All;
	extern bool FreeZodiac;
	extern bool UIScale;
	extern float UIScaleResolution;
	extern int ShadowQuality;
	extern int ImposterQuality;
}