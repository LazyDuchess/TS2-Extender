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
	extern int DesignToolPrice;
	extern bool Sims3Camera;
	extern float Sims3CameraX;
	extern float Sims3CameraY;
	extern bool GenderedThumbnails;
	extern bool OceanReflections;
	extern bool Splash;
	extern float SplashVerticalCoverage;
	extern bool ExtendedSimAntics;
	extern bool SingleCore;
	extern bool DisableGroupsCache;
}