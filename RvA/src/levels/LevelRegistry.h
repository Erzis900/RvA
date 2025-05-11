#pragma once

#include "levels/LevelInfo.h"

#include <string>
#include <unordered_map>

class LevelRegistry {
public:
	void registerLevel(std::string id, LevelInfo levelInfo);
	const LevelInfo* getLevel(const std::string& id) const;

	const auto& getLevels() const {
		return m_levels;
	}

private:
	std::unordered_map<std::string, LevelInfo> m_levels;
};
