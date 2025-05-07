#pragma once

#include "Enemy.h"
#include "utilities/CallbackRegistry.h"

#include <functional>
#include <memory>
#include <vector>

class Atlas;
class CollisionSystem;
class GameRegistry;

struct EnemyDestroyedInfo {
	const EnemyTypeInfo* info{};
	Vector2 position{};
	DamageSource damageSource{};
};

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
	CallbackHandle onEnemiesDestroyed(std::function<void(const std::vector<EnemyDestroyedInfo>&)> callback);

private:
	void spawnEnemy();

	std::vector<std::unique_ptr<Enemy>> m_enemies;
	CallbackRegistry<const std::vector<EnemyDestroyedInfo>&> m_onEnemiesDestroyedCallbacks;
	std::vector<EnemyDestroyedInfo> m_enemyDestroyedInfos;
	float m_spawnTimer{};
	float m_spawnInterval{1.f};

	const GameRegistry& m_gameRegistry;
	CollisionSystem& m_collisionSystem;
};
