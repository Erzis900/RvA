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

using GameAction = std::variant<BulletSpawnAction, EnemySpawnAction>;
using GameActions = std::vector<GameAction>;
