#pragma once

#include "levels/LevelInfo.h"

#include <raylib.h>
#include <string>
#include <variant>
#include <vector>

struct BulletSpawnAction {
	std::string bulletType;
	Vector2 position;
};

struct EnemySpawnAction {
	std::string enemyType;
	int row{};
	int column{};
};

struct PortalSpawnAction {
	int inRow{};
	int inCol{};

	int outRow{};
	int outCol{};
};

struct DefenderSpawnAction {
	std::string id;
	int row{};
	int column{};
	bool isEnabled{};
};

struct WinAction {};

struct EndAction {};

struct KillRandomEnemy {};

struct AddBattery {
	int amount;
};

struct ChangeSpeed {};

struct LoseAction {};

using MessageAction = MessageOperation;
using HUDAction = HUDOperation;
using DefenderPickerAction = DefenderPickerOperation;
using EnableDefenderAction = EnableDefenderOperation;
using UpdateValidCellAction = UpdateValidCellOperation;
using ClearAllEntityAction = ClearAllEntityOperation;
using ClearEntityAction = ClearEntityOperation;

using GameActionVariant = std::variant<std::monostate,
									   BulletSpawnAction,
									   EnemySpawnAction,
									   DefenderSpawnAction,
									   PortalSpawnAction,
									   WinAction,
									   LoseAction,
									   EndAction,
									   MessageAction,
									   HUDAction,
									   DefenderPickerAction,
									   EnableDefenderAction,
									   UpdateValidCellAction,
									   KillRandomEnemy,
									   AddBattery,
									   ChangeSpeed,
									   ClearEntityAction,
									   ClearAllEntityAction>;

struct [[nodiscard]] GameAction : public GameActionVariant {
	using GameActionVariant::variant;
};

using GameActions = std::vector<GameAction>;
