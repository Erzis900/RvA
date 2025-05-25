#include "Config.h"

#include "constants.h"

#include <fstream>
#include <iostream>

// TODO figure out how to fix spdlog
// #include <spdlog/spdlog.h>

Config::Config() {
	std::ifstream configFile("assets/config/options.json");
	/*if (!configFile.is_open()) {
		spdlog::error("Failed to open config file");
		return;
	}
	spdlog::info("Loaded config file");

	try {
		m_data = nlohmann::json::parse(configFile);
		spdlog::info("Parsed {}", configFile);
	} catch (const nlohmann::json::parse_error& e) {
		spdlog::error("Failed to parse {}. Error: {}", configFile, e.what());
		std::exit(EXIT_FAILURE);
	}*/

	m_data = nlohmann::json::parse(configFile);

#ifndef WEB_MODE
	options.isFullscreen = m_data["fullscreen"];
	options.isMusicEnabled = m_data["musicEnabled"];
	options.isSoundEnabled = m_data["soundEnabled"];
	options.isTutorialEnabled = m_data["tutorialEnabled"];
	options.sfxVolume = m_data["sfxVolume"];
	options.musicVolume = m_data["musicVolume"];
	options.masterVolume = m_data["masterVolume"];
	options.cheatEnabled = m_data["cheatEnabled"];
#endif
}

void Config::save() {
	std::ofstream configFile("assets/config/options.json");
	if (!configFile.is_open()) {
		std::cerr << "Failed to open config file for writing" << std::endl;
		return;
	}
	m_data["fullscreen"] = options.isFullscreen;
	m_data["musicEnabled"] = options.isMusicEnabled;
	m_data["soundEnabled"] = options.isSoundEnabled;
	m_data["tutorialEnabled"] = options.isTutorialEnabled;
	m_data["sfxVolume"] = options.sfxVolume;
	m_data["musicVolume"] = options.musicVolume;
	m_data["masterVolume"] = options.masterVolume;
	m_data["cheatEnabled"] = options.cheatEnabled;

	configFile << m_data.dump(4);
	configFile.close();
}
