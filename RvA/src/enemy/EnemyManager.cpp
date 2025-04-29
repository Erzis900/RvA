#include "EnemyManager.h"
#include <raylib.h>
#include "Game.h"
#include "EnemyTypes.h"
#include "constants.h"
#include <ranges>

EnemyManager::EnemyManager(Game& game) : m_game(game)
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

    // Remove enemies reaching the left side of the grid
    std::erase_if(m_enemies, [](const auto& enemy) {
        return enemy->getPosition().x < CELL_SIZE * 1.5f;
    });
    
    // Remove enemies which have been destroyed ( hp == 0 )
    auto numberOfDestroyedEnemies = std::erase_if(m_enemies, [](const auto& enemy) {
        return enemy->getHp() <= 0.f;
    });
    if (numberOfDestroyedEnemies > 0) {
        notifyEnemiesDestroyed(numberOfDestroyedEnemies);
    }
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

    EnemyType type;
    if (selectedName == "b1_alien_walk") {
        type = EnemyType::B1;
    }
    else if (selectedName == "b2_alien_walk") {
        type = EnemyType::B2;
    }

    int randomRow = GetRandomValue(0, ROWS - 1);
    float x = TEX_WIDTH - CELL_SIZE * 2.f;
    int y = (randomRow + 1) * CELL_SIZE;

    m_enemies.push_back(std::make_unique<Enemy>(Vector2{ x, float(y) }, type, m_game.getAtlas(), randomRow));
}

void EnemyManager::onEnemiesDestroyed(std::function<void(int)> callback)
{
    m_onEnemiesDestroyedCallback = callback;
}

void EnemyManager::notifyEnemiesDestroyed(int numberOfDestroyedEnemies)
{
    m_onEnemiesDestroyedCallback(numberOfDestroyedEnemies);
}