#pragma once

#include <portal/PortalTypes.h>
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
	int row;
	int column;
};

struct PortalSpawnAction {
	int inRow;
	int inCol;

	int outRow;
	int outCol;
};

struct DefenderSpawnAction {
	std::string id;
	int row;
	int column;
};

struct WinAction {};

struct LoseAction {};

struct [[nodiscard]] GameAction : public std::variant<std::monostate, BulletSpawnAction, EnemySpawnAction, DefenderSpawnAction, PortalSpawnAction, WinAction, LoseAction> {
	using std::variant<std::monostate, BulletSpawnAction, EnemySpawnAction, DefenderSpawnAction, PortalSpawnAction, WinAction, LoseAction>::variant;
};

using GameActions = std::vector<GameAction>;
