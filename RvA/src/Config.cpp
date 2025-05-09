#include "Config.h"

#include <fstream>
#include <iostream>

// TODO figure out how to fix spdlog
// #include <spdlog/spdlog.h>

Config::Config() {
	std::ifstream configFile("config/options.json");
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

	options.isFullscreen = m_data["fullscreen"];
	options.isMusic = m_data["music"];
	options.isSound = m_data["sound"];
}

void Config::save() {
	std::ofstream configFile("config/options.json");
	if (!configFile.is_open()) {
		std::cerr << "Failed to open config file for writing" << std::endl;
		return;
	}
	m_data["fullscreen"] = options.isFullscreen;
	m_data["music"] = options.isMusic;
	m_data["sound"] = options.isSound;

	configFile << m_data.dump(4);
	configFile.close();
}
