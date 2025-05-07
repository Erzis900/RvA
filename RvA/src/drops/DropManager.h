#pragma once

#include "DropTypes.h"
#include "utilities/CallbackRegistry.h"

#include <memory>
#include <vector>

struct Vector2;
class GameRegistry;
class CollisionSystem;

struct CollectedDrop {
	const DropTypeInfo* info{};
	int amount{};
};

class DropManager {
public:
	DropManager(const GameRegistry& gameRegistry, CollisionSystem& collisionSystem);

	void spawnDrop(const DropTypeInfo* drop, const Vector2& position, int amount);

	void clear();
	void update(float dt);
	void draw(Atlas& atlas);

	CallbackHandle onDropCollected(std::function<void(const std::vector<CollectedDrop>&)> callback) {
		return m_onDropsCollectedCallbacks.registerCallback(std::move(callback));
	}

private:
	std::vector<std::unique_ptr<Drop>> m_drops;
	std::vector<CollectedDrop> m_collectedDrops;
	CallbackRegistry<const std::vector<CollectedDrop>&> m_onDropsCollectedCallbacks;
};
