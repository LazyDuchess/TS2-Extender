#pragma once

namespace Config {
	void Load();

	extern bool Console;
	extern bool SkipIntro;
	extern bool FixRNG;
	extern bool FixOFBUniform;
	extern bool FixPinkFlashing;
	extern bool FixMakeupLag;
	extern bool FixPoolShadows;
	extern bool FixOutdoorShadows;
	extern bool ExtendedLua;
	extern bool Separates4All;
	extern bool FreeZodiac;
	extern bool UIScale;
	extern float UIScaleResolution;
}