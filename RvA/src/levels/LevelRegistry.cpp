#include "LevelRegistry.h"

void LevelRegistry::registerLevel(std::string id, LevelInfo levelInfo) {
	m_levels[std::move(id)] = std::move(levelInfo);
}

const LevelInfo* LevelRegistry::getLevel(const std::string& id) const {
	if (auto it = m_levels.find(id); it != m_levels.end()) {
		return &it->second;
	}
	return nullptr;
}
