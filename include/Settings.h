#pragma once
#include "inih/INIReader.h"
#include <cstdint>

class Settings {
public:
    struct ModPreferences {
        uint32_t matchDuration;
    };

    static ModPreferences modPreferences;

	static bool ParseIniFile(const INIReader& reader);
};

inline bool Settings::ParseIniFile(const INIReader& reader) {
	if (reader.ParseError() < 0) {
		std::cout << "Can't load 'EldenWarfare.ini'\n";
		return false;
	}

	modPreferences.matchDuration = reader.GetUnsigned("Match Settings", "MatchDuration", 300);

	return true;
}