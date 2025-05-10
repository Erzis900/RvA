#pragma once

#include "atlas/Atlas.h"
#include "utilities/ConfigCondition.h"
#include "utilities/ConfigValue.h"

#include <string>

using EnemySelection = Selection<std::string>;

struct SpawnEnemyOperation {
	ConfigValue<int> row{};
	ConfigValue<int> column{};
	ConfigValue<std::string> type{};
};

struct SpawnEnemyBurstOperation {
	ConfigValue<int> amount{};
	ConfigValue<float> interval{};
	ConfigValue<int> row{};
	ConfigValue<int> column{};
	ConfigValue<std::string> type{};
};

using KeyframeOperation = std::variant<SpawnEnemyOperation, SpawnEnemyBurstOperation>;

struct Keyframe {
	float time;
	KeyframeOperation action;
};

struct Timeline {
	std::vector<Keyframe> keyframes;
};

struct AllWavesGoneCondition {};

struct BatteryLevelCondition {
	ConfigCondition<float> batteryLevel{};
};

using LevelCondition = std::variant<BatteryLevelCondition, AllWavesGoneCondition>;

struct LevelInfo {
	std::string name;
	int startingScraps{};
	float maxBatteryCharge{};
	LevelCondition winCondition{};
	LevelCondition loseCondition{};
	SpriteInfo* background{};
	Timeline timeline{};
};
