#pragma once

#include "levels/LevelInfo.h"

/*
 * A LevelData contains the info about the current level progression
 */
struct LevelData {
	const LevelInfo* info{};
	float time{};
	float gameSpeed{1.f};
	int nextKeyframe{};

	int scraps{};
	float batteryCharge{};
	int enemyCount{};

	int numberOfEnabledDefenders{};
	std::unordered_map<std::string, int> enabledDefenders;
	std::array<std::array<bool, COLS>, ROWS> validBuildingCells;

	/*
	 * In most cases we don't want to immediatly trigger a WinAction but wait few seconds
	 */
	bool isWinningCountdownActive{};
	float countdownToWin{};

	int getEnableDefenderAmount(const std::string& id) const {
		auto it = enabledDefenders.find(id);
		if (it != enabledDefenders.end()) {
			return it->second;
		}
		return 0;
	}
};
