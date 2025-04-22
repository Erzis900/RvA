#pragma once
#include <vector>
#include <memory>
#include "Enemy.h"
#include <map>

class Game;

class EnemyManager
{
public:
    EnemyManager(Game& game, int rows, int cellSize);

    void update(float dt);
    void draw();

private:
    void spawnEnemy();

    std::vector<std::unique_ptr<Enemy>> m_enemies;
    float m_spawnTimer;
    float m_spawnInterval;

    int m_rows;
    int m_cellSize;

    Game& m_game;
    std::map<std::string, float> m_spawnData;
};
