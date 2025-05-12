#pragma once

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

struct DefenderSpawnAction {
	std::string id;
	int row;
	int column;
};

struct WinAction {};

struct LoseAction {};

using GameAction = std::variant<BulletSpawnAction, EnemySpawnAction, DefenderSpawnAction, WinAction, LoseAction>;
using GameActions = std::vector<GameAction>;
