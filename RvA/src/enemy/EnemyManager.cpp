#include "EnemyManager.h"
#include <raylib.h>
#include "Game.h"
#include "EnemyTypes.h"

EnemyManager::EnemyManager(Game& game)
    :m_game(game), m_spawnTimer(0.0f), m_spawnInterval(1.f)
{
    m_spawnData = {
        { "b1_alien_walk", 0.7f },
        { "b2_alien_walk", 0.3f },
    };
}

void EnemyManager::update(float dt, int cellSize, int rows)
{
    m_spawnTimer += dt;
    if (m_spawnTimer >= m_spawnInterval)
    {
        spawnEnemy(cellSize, rows);
        m_spawnTimer = 0.f;
    }

    for (auto& enemy : m_enemies)
    {
        enemy->update(dt);
    }

    m_enemies.erase(
        std::remove_if(m_enemies.begin(), m_enemies.end(),
            [cellSize](const std::unique_ptr<Enemy>& enemy)
            {
                return enemy->getPosition().x < cellSize * 1.5f || enemy->getHp() <= 0.f;

            }),
        m_enemies.end()
    );
}

void EnemyManager::draw(int cellSize)
{
    for (auto& enemy : m_enemies)
    {
        enemy->draw(m_game, cellSize);
    }
}

void EnemyManager::spawnEnemy(int cellSize, int rows)
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

    EnemyType type;
    if (selectedName == "b1_alien_walk") {
        type = EnemyType::B1;
    }
    else if (selectedName == "b2_alien_walk") {
        type = EnemyType::B2;
    }

    int randomRow = GetRandomValue(0, rows - 1);
    float x = m_game.getTexSize().x - cellSize * 2.f;
    int y = (randomRow + 1) * cellSize;

    m_enemies.push_back(std::make_unique<Enemy>(Vector2{ x, float(y) }, type, m_game.getAtlas(), randomRow));
}