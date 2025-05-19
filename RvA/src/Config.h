#pragma once
#include <nlohmann/json.hpp>

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
	} options;

	json getData() const {
		return m_data;
	}

	void save();

private:
	json m_data;
};
