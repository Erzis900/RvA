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
	} options;

	json getData() const {
		return m_data;
	}

	void save();

private:
	json m_data;
};
