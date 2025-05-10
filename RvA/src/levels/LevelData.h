#pragma once

#include "levels/LevelInfo.h"

/*
 * A LevelData contains the info about the current level progression
 */
struct LevelData {
	const LevelInfo* info{};
	float time{};
	int nextKeyframe{};

	int scraps{};
	float batteryCharge{};
	int enemyCount{};
};
