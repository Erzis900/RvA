#include "EnemyManager.h"
#include <raylib.h>
#include <raymath.h>
#include "Game.h"
#include "EnemyTypes.h"
#include "constants.h"
#include <cassert>

void EnemyTypeRegistry::registerEnemyType(EnemyTypeInfo typeInfo)
{
    m_enemyTypes.insert({ typeInfo.type, std::move(typeInfo) });
}

const EnemyTypeInfo* EnemyTypeRegistry::getEnemyTypeInfo(EnemyType type) const
{
    auto itr = m_enemyTypes.find(type);
    return (itr != m_enemyTypes.end()) ? &itr->second : nullptr;
}

EnemyManager::EnemyManager(Game& game, const EnemyTypeRegistry& enemyTypeRegistry, DefenderManager& defenderManager)
    : m_game(game), m_enemyTypeRegistry(enemyTypeRegistry), m_defenderManager(defenderManager)
{
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
        manageDefenderCollisions(*enemy);
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
        notifyEnemiesDestroyed(static_cast<int>(numberOfDestroyedEnemies));
    }
}

void EnemyManager::draw()
{
    for (auto& enemy : m_enemies)
    {
        enemy->draw(m_game);

    }
}

Enemy* EnemyManager::findClosestEnemy(const Vector2& position)
{
    Enemy* closestEnemy = nullptr;
    float closestDistSq = std::numeric_limits<float>::max();

    for (auto& enemy : m_enemies)
    {
        float distSq = Vector2LengthSqr(Vector2Subtract(enemy->getCenteredPosition(), position));
        if (distSq < closestDistSq)
        {
            closestDistSq = distSq;
            closestEnemy = enemy.get();
        }
    }

    return closestEnemy;
}

void EnemyManager::spawnEnemy()
{
    float randomValue = GetRandomValue(0, 100) / 100.0f;

    float cumulativeChance = 0.0f;
    const auto& typeInfos = m_enemyTypeRegistry.getEnemyTypeInfos();
    const EnemyTypeInfo* enemyTypeInfo = &(typeInfos.begin()->second);
    for (const auto& [type, typeInfo] : typeInfos)
    {
        cumulativeChance += typeInfo.spawnChance;
        if (randomValue <= cumulativeChance)
        {
            enemyTypeInfo = &typeInfo;
            break;
        }
    }

    int randomRow = GetRandomValue(0, ROWS - 1);
    float x = TEX_WIDTH - CELL_SIZE * 2.f;
    int y = (randomRow + 1) * CELL_SIZE;

    m_enemies.push_back(std::make_unique<Enemy>(Vector2{ x, float(y) }, enemyTypeInfo, m_game.getAtlas(), randomRow));
}

void EnemyManager::onEnemiesDestroyed(std::function<void(int)> callback)
{
    m_onEnemiesDestroyedCallback = callback;
}

void EnemyManager::notifyEnemiesDestroyed(int numberOfDestroyedEnemies)
{
    m_onEnemiesDestroyedCallback(numberOfDestroyedEnemies);
}

void EnemyManager::manageDefenderCollisions(Enemy& enemy)
{
    bool collideWithDefenders{ false };
    for (auto& defender : m_defenderManager.getDefenders())
    {
        if (defender->row == enemy.getRow())
        {
            if (enemy.getPosition().x <= defender->position.x + CELL_SIZE && enemy.getPosition().x > defender->position.x)
            {
                if (enemy.getState() == EnemyState::ReadyToAttack)
                {
                    defender->hp -= static_cast<int>(enemy.getDamage());
                    enemy.setState(EnemyState::PrepareToAttack);
                }
                enemy.setState(EnemyState::PrepareToAttack);

                collideWithDefenders = true;
                break; // as soon as we collide with the first defender we move on
            }
        }
    }

    if (!collideWithDefenders)
    {
        enemy.setState(EnemyState::Moving);
    }
}
