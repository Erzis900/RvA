#pragma once

#include "GUI/Widgets.h"
#include "atlas/Atlas.h"
#include "constants.h"
#include "utilities/ConfigCondition.h"
#include "utilities/ConfigValue.h"

#include <optional>
#include <string>

struct LevelData;

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
	bool enabled{true};
};

struct SpawnEntityBurstOperation {
	ConfigValue<int> amount{};
	ConfigValue<float> interval{};
	ConfigValue<int> row{};
	ConfigValue<int> column{};
	ConfigValue<std::string> id{};
	EntityType type{EntityType::Enemy};
};

struct EnableDefenderOperation {
	bool enable{};
	ConfigValue<int> targetIndex{};
	ConfigValue<std::string> targetId{};
};

struct MessageOperation {
	std::string text{};
	float fontSize{FONT_SMALL};
	Vector2 highlightPosition{};
	Vector2 highlightSize{};
	std::optional<Vector2> textPosition{};
	std::optional<HAlign> textHAlign{};
	std::optional<VAlign> textVAlign{};
	float timer;
};

// A special operation which accept a function.
// It blocks the timeline till the function returns true
struct CheckOperation {
	std::function<bool(const LevelData&)> check;
};

enum class HUDOperationType {
	Disable,
	Enable,
	ShowResources,
	HideResources,
	ShowDefenderPicker,
	HideDefenderPicker,
	ShowTimeline,
	HideTimeline,
	HidePlate,
	ShowPlate,
	HideDefenderOverlay,
	ShowDefenderOverlay,
	ShowSkipButton,
	HideSkipButton
};

struct HUDOperation {
	HUDOperationType type{};
};

enum class DefenderPickerOperationType {
	AddItem,
	Reset,
};

struct DefenderPickerOperation {
	DefenderPickerOperationType type{};
	std::string id{};
};

struct FlagTimelineOperation {
	std::string icon{};
};

struct CellPosition {
	ConfigValue<int> row{};
	ConfigValue<int> column{};
};

struct UpdateValidCellOperation {
	bool clearAll{};
	std::vector<CellPosition> cells{};
};

using KeyframeOperation = std::variant<SpawnEntityOperation,
									   SpawnEntityBurstOperation,
									   MessageOperation,
									   HUDOperation,
									   DefenderPickerOperation,
									   CheckOperation,
									   FlagTimelineOperation,
									   EnableDefenderOperation,
									   UpdateValidCellOperation>;

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
	std::string id;
	std::string name;
	std::string musicId;
	int startingScraps{};
	float maxBatteryCharge{};
	float winCountdownDuration{};
	LevelCondition winCondition{};
	LevelCondition loseCondition{};
	const SpriteInfo* groundBackground{};
	const SpriteInfo* topBackground{};
	bool isProgressionLevel{true};
	Timeline timeline{};
};
