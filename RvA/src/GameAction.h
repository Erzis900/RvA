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

using GameAction = std::variant<BulletSpawnAction, EnemySpawnAction, PortalSpawnAction>;
using GameActions = std::vector<GameAction>;
