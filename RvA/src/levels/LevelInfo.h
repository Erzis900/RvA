#pragma once

#include "atlas/Atlas.h"
#include "utilities/ConfigCondition.h"
#include "utilities/ConfigValue.h"

#include <string>

enum class EntityType {
	Defender,
	Enemy,
};

using EntitySelection = Selection<std::string>;

struct SpawnEntityOperation {
	ConfigValue<int> row{};
	ConfigValue<int> column{};
	ConfigValue<std::string> id{};
	EntityType type{EntityType::Enemy};
};

struct SpawnEntityBurstOperation {
	ConfigValue<int> amount{};
	ConfigValue<float> interval{};
	ConfigValue<int> row{};
	ConfigValue<int> column{};
	ConfigValue<std::string> id{};
	EntityType type{EntityType::Enemy};
};

struct TutorialOperation {
	std::string text{};
	Vector2 highlightPosition{};
	Vector2 highlightSize{};
};

enum class HUDOperationType {
	Disable,
	Enable,
};

struct HUDOperation {
	HUDOperationType type{};
};

using KeyframeOperation = std::variant<SpawnEntityOperation, SpawnEntityBurstOperation, TutorialOperation, HUDOperation>;

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
	float winCountdownDuration{};
	LevelCondition winCondition{};
	LevelCondition loseCondition{};
	const SpriteInfo* groundBackground{};
	const SpriteInfo* topBackground{};
	Timeline timeline{};
};
