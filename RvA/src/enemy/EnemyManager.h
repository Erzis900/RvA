#pragma once
#include <vector>
#include <memory>
#include "Enemy.h"
#include <map>

class Game;

class EnemyManager
{
public:
    EnemyManager(Game& game);

    void update(float dt);
    void draw();

    const std::vector<std::unique_ptr<Enemy>>& getEnemies() const
    {
        return m_enemies;
    }

private:
    void spawnEnemy();

    std::vector<std::unique_ptr<Enemy>> m_enemies;
    float m_spawnTimer;
    float m_spawnInterval;

    Game& m_game;
    std::map<std::string, float> m_spawnData;
};
