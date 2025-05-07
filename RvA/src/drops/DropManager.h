#pragma once

#include "DropTypes.h"

#include <memory>
#include <vector>

struct Vector2;
class GameRegistry;
class CollisionSystem;

class DropManager {
public:
	DropManager(GameRegistry& gameRegistry, CollisionSystem& collisionSystem);

	void spawnDrop(const Vector2& position);

	void update(float dt);

private:
	std::vector<std::unique_ptr<Drop>> m_drops;
};
