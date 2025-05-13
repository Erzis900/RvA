#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Config {
public:
	Config();

	struct options {
		bool isFullscreen{false};
		bool isMusic{true};
		bool isSound{true};
		//TODO How do I add float?
		//float musicVolume{0.2f};
		//float soundVolume{0.2f};
	} options;

	json getData() const {
		return m_data;
	}

	void save();

private:
	json m_data;
};
