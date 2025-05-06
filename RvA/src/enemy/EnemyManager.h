#pragma once

#include "Enemy.h"

#include <functional>
#include <memory>
#include <vector>

class Atlas;
class CollisionSystem;
class GameRegistry;

class EnemyManager {
public:
	EnemyManager(const GameRegistry& gameRegistry, CollisionSystem& collisionSystem);

	void clear();
	void update(float dt);
	void draw(Atlas& atlas);

	const auto& getEnemies() const {
		return m_enemies;
	}

	Enemy* findClosestEnemy(const Vector2& position, bool filterDead);

	// Register a lambda when enemies are destroyed. The callback receives the number of enemies destroyed.
	// TODO(Gerark) - We're probably going to change this in the future to better know which enemies have been destroyed
	void onEnemiesDestroyed(std::function<void(int)> callback);

private:
	void spawnEnemy();
	void notifyEnemiesDestroyed(int numberOfDestroyedEnemies);

	std::vector<std::unique_ptr<Enemy>> m_enemies;
	std::function<void(int)> m_onEnemiesDestroyedCallback;
	float m_spawnTimer{};
	float m_spawnInterval{1.f};

	const GameRegistry& m_gameRegistry;
	CollisionSystem& m_collisionSystem;
};
