#pragma once

#include "atlas/Atlas.h"
#include "utilities/ConfigCondition.h"
#include "utilities/ConfigValue.h"

#include <string>

using EnemySelection = Selection<std::string>;

struct SpawnEnemy {
	ConfigValue<int> row{};
	ConfigValue<int> column{};
	ConfigValue<std::string> type{};
};

struct SpawnEnemyBurst {
	ConfigValue<int> amount{};
	ConfigValue<float> interval{};
	ConfigValue<int> row{};
	ConfigValue<int> column{};
	ConfigValue<std::string> type{};
};

using KeyframeAction = std::variant<SpawnEnemy, SpawnEnemyBurst>;

struct Keyframe {
	float time;
	KeyframeAction action;
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
	int rowCount{};
	int columnCount{};
	Vector2 gridOffset{};
	LevelCondition winCondition{};
	LevelCondition loseCondition{};
	SpriteInfo* background{};
	Timeline timeline{};
};
