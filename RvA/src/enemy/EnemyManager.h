#pragma once
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include "Enemy.h"

class Game;
class DefenderManager;

class EnemyManager
{
public:
    EnemyManager(Game& game, DefenderManager& defenderManager);

    void update(float dt);
    void draw();

    const std::vector<std::unique_ptr<Enemy>>& getEnemies() const
    {
        return m_enemies;
    }

    // Register a lambda when enemies are destroyed. The callback receives the number of enemies destroyed.
    // TODO(Gerark) - We're probably going to change this in the future to better know which enemies have been destroyed
    void onEnemiesDestroyed(std::function<void(int)> callback);

private:
    void spawnEnemy();
    void notifyEnemiesDestroyed(int numberOfDestroyedEnemies);
    void manageDefenderCollisions(Enemy& enemy);

    std::vector<std::unique_ptr<Enemy>> m_enemies;
    std::function<void(int)> m_onEnemiesDestroyedCallback;
    float m_spawnTimer{};
    float m_spawnInterval{ 1.f };

    Game& m_game;
    DefenderManager& m_defenderManager;
    std::map<std::string, float> m_spawnData;
};
