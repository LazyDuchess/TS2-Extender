#include "config.h"
#include "ini.h"
#include <string>
#include "Logging.h"

namespace Config {
	static const char* ConfigFilename = "TS2Extender.ini";
	static mINI::INIStructure Ini;
	bool Console;
	bool SkipIntro;
	bool FixRNG;
	bool FixOFBUniform;
	bool ExtendedLua;

	static bool Has(const std::string& section, const std::string& key) {
		if (!Ini[section].has(key)) return false;
		return true;
	}

	static std::string GetString(const std::string& section, const std::string& key, std::string defaultValue) {
		if (!Has(section, key)) {
			Ini[section][key] = defaultValue;
			return defaultValue;
		}
		return Ini[section][key];
	}

	static int GetInt(const std::string& section, const std::string& key, int defaultValue) {
		if (!Has(section, key)) {
			Ini[section][key] = std::to_string(defaultValue);
			return defaultValue;
		}
		return std::stoi(Ini[section][key]);
	}

	static float GetFloat(const std::string& section, const std::string& key, float defaultValue) {
		if (!Has(section, key)) {
			Ini[section][key] = std::to_string(defaultValue);
			return defaultValue;
		}
		return std::stof(Ini[section][key]);
	}

	static bool GetBool(const std::string& section, const std::string& key, bool defaultValue) {
		std::string value = GetString(section, key, defaultValue == false ? "false" : "true");
		if (value == "true" || value == "1") return true;
		return false;
	}

	void Load() {
		bool iniExisted = false;
		mINI::INIFile file(ConfigFilename);

		Log("Reading config from %s.\n", ConfigFilename);

		if (file.read(Ini))
			iniExisted = true;

		Console = GetBool("General", "Console", false);
		SkipIntro = GetBool("General", "SkipIntro", true);
		FixRNG = GetBool("Fixes", "FixRNG", true);
		FixOFBUniform = GetBool("Fixes", "FixOFBUniform", true);
		ExtendedLua = GetBool("Advanced", "ExtendedLua", true);

		if (!iniExisted) {
			Log("Default config is being written to %s cause it didn't exist.\n", ConfigFilename);
			file.generate(Ini, true);
		}
		else
		{
			file.write(Ini, true);
		}
	}
}