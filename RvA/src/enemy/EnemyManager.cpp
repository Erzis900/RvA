#include "EnemyManager.h"
#include <raylib.h>
#include "Game.h"
#include <iostream>

EnemyManager::EnemyManager(Game& game, int rows, int cellSize)
    : m_rows(rows), m_cellSize(cellSize),
	m_spawnTimer(0.0f), m_spawnInterval(1.f), m_game(game)
{
    m_spawnData = {
        { "b1_alien_walk", 0.7f },
        { "b2_alien_walk", 0.3f },
    };
}

void EnemyManager::update(float dt)
{
    m_spawnTimer += dt;
    if (m_spawnTimer >= m_spawnInterval)
    {
        spawnEnemy();
        m_spawnTimer = 0.f;
    }

    for (auto& enemy : m_enemies)
    {
        enemy->update(dt);
    }

    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [this](const std::unique_ptr<Enemy>& enemy)
            {
                return enemy->getPosition().x < m_cellSize * 1.5f;
            }),
        m_enemies.end()
    );
}

void EnemyManager::draw()
{
    for (auto& enemy : m_enemies)
    {
        enemy->draw(m_game);
    }
}

void EnemyManager::spawnEnemy()
{
    float randomValue = GetRandomValue(0, 100) / 100.0f;

    std::string selectedName;
    float cumulativeChance = 0.0f;

    for (const auto& spawn : m_spawnData)
    {
        cumulativeChance += spawn.second;
        if (randomValue <= cumulativeChance)
        {
            selectedName = spawn.first;
            break;
        }
    }

    int randomRow = GetRandomValue(0, m_rows - 1);
    float x = m_game.getTexSize().x - m_cellSize * 2.f;
    int y = (randomRow + 1) * m_cellSize;

    m_enemies.push_back(std::make_unique<Enemy>(Vector2{ x, float(y) }, selectedName, m_game.getAtlas()));
}