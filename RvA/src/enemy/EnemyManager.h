#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

#include "Enemy.h"

class Atlas;
class CollisionSystem;

class EnemyTypeRegistry
{
public:
    void registerEnemyType(EnemyTypeInfo typeInfo);
    const EnemyTypeInfo* getEnemyTypeInfo(EnemyType type) const;
    const auto& getEnemyTypeInfos() const { return m_enemyTypes; };

private:
    std::unordered_map<EnemyType, EnemyTypeInfo> m_enemyTypes;
};

class EnemyManager
{
public:
    EnemyManager(const EnemyTypeRegistry& enemyTypeRegistry, CollisionSystem& collisionSystem);

    void clear();
    void update(float dt);
    void draw(Atlas& atlas);

    const auto& getEnemies() const { return m_enemies; }
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
    float m_spawnInterval{ 100.f };

    const EnemyTypeRegistry& m_enemyTypeRegistry;
    CollisionSystem& m_collisionSystem;
};
