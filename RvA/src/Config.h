#pragma once

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

class Config {
public:
	Config();

	struct options {
		bool isFullscreen{false};
		bool isMusicEnabled{true};
		bool isSoundEnabled{true};
		bool isTutorialEnabled{true};
		float sfxVolume{0.5};
		float musicVolume{0.5};
		float masterVolume{0.5};
		bool cheatEnabled{false};
		std::string levelId{"none"};
	} options;

	json getData() const {
		return m_data;
	}

	void save();

	bool hasLevel() const;

private:
	json m_data;
};
