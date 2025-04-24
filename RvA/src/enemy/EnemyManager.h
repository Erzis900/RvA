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

    void update(float dt, int cellSize, int rows);
    void draw(int cellSize);

private:
    void spawnEnemy(int cellSize, int rows);

    std::vector<std::unique_ptr<Enemy>> m_enemies;
    float m_spawnTimer;
    float m_spawnInterval;

    Game& m_game;
    std::map<std::string, float> m_spawnData;
};
