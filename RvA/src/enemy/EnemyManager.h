#pragma once

#include "Enemy.h"
#include "utilities/CallbackRegistry.h"

#include <functional>
#include <memory>
#include <vector>

class Atlas;
class CollisionSystem;
class GameRegistry;
class MusicManager;

struct EnemyDestroyedInfo {
	const EnemyTypeInfo* info{};
	Vector2 position{};
	DamageSource damageSource{};
};

class EnemyManager {
public:
	EnemyManager(const GameRegistry& gameRegistry, CollisionSystem& collisionSystem, MusicManager& musicManager);

	void clear();
	GameActions update(float dt);
	void draw(Atlas& atlas);

	void spawnEnemy(const EnemyTypeInfo* info, int row, int column);

	const auto& getEnemies() const {
		return m_enemies;
	}

	Enemy* findClosestEnemy(const Vector2& position, bool filterDead);
	CallbackHandle onEnemiesDestroyed(std::function<void(const std::vector<EnemyDestroyedInfo>&)> callback);
	CallbackHandle onEnemiesClear(std::function<void()> callback);

private:
	std::vector<std::unique_ptr<Enemy>> m_enemies;
	CallbackRegistry<const std::vector<EnemyDestroyedInfo>&> m_onEnemiesDestroyedCallbacks;
	CallbackRegistry<> m_onEnemiesClearCallbacks;
	std::vector<EnemyDestroyedInfo> m_enemyDestroyedInfos;

	const GameRegistry& m_gameRegistry;
	CollisionSystem& m_collisionSystem;
	MusicManager& m_musicManager;
};
