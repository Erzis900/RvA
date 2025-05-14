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

struct LoseAction {};

using TutorialAction = TutorialOperation;
using HUDAction = HUDOperation;
using DefenderPickerAction = DefenderPickerOperation;

using GameActionVariant =
	std::variant<std::monostate, BulletSpawnAction, EnemySpawnAction, DefenderSpawnAction, PortalSpawnAction, WinAction, LoseAction, TutorialAction, HUDAction, DefenderPickerAction>;

struct [[nodiscard]] GameAction : public GameActionVariant {
	using GameActionVariant::variant;
};

using GameActions = std::vector<GameAction>;
