#include "EnemyManager.h"
#include <raylib.h>
#include <raymath.h>
#include "Game.h"
#include "EnemyTypes.h"
#include "constants.h"

void EnemyTypeRegistry::registerEnemyType(EnemyTypeInfo typeInfo)
{
    m_enemyTypes.insert({ typeInfo.type, std::move(typeInfo) });
}

const EnemyTypeInfo* EnemyTypeRegistry::getEnemyTypeInfo(EnemyType type) const
{
    auto itr = m_enemyTypes.find(type);
    return (itr != m_enemyTypes.end()) ? &itr->second : nullptr;
}

EnemyManager::EnemyManager(Game& game, const EnemyTypeRegistry& enemyTypeRegistry, CollisionSystem& collisionSystem)
    : m_game(game), m_enemyTypeRegistry(enemyTypeRegistry), m_collisionSystem(collisionSystem)
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

    auto numberOfDestroyedEnemies = 0;
    for (auto it = m_enemies.begin(); it != m_enemies.end(); )
    {
        auto& enemy = *it;
        enemy->update(dt);

        if (!enemy->isDying())
        {
            if (enemy->getHp() <= 0)
            {
                enemy->setState(EnemyState::Dying);
                m_collisionSystem.destroyCollider(enemy->getColliderHandle());
            }
            else
            {
                m_collisionSystem.updateCollider(enemy->getColliderHandle(), enemy->calculateBoundingBox());
            }
        }

        if (enemy->getState() == EnemyState::Dead)
        {
            numberOfDestroyedEnemies += enemy->getLatestDamageApplied().source == DamageSource::Bullet ? 1 : 0;
            it = m_enemies.erase(it);
        }
        else
        {
            ++it;
        }
    }

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

Enemy* EnemyManager::findClosestEnemy(const Vector2& position, bool filterDead)
{
    Enemy* closestEnemy = nullptr;
    float closestDistSq = std::numeric_limits<float>::max();

    for (auto& enemy : m_enemies)
    {
        if (!filterDead || enemy->isDying() != filterDead)
        {
            float distSq = Vector2LengthSqr(Vector2Subtract(enemy->getCenteredPosition(), position));
            if (distSq < closestDistSq)
            {
                closestDistSq = distSq;
                closestEnemy = enemy.get();
            }
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
    int y = (randomRow + 1) * CELL_SIZE - 5;

    auto enemy = std::make_unique<Enemy>(Vector2{ x, float(y) }, enemyTypeInfo, m_game.getAtlas(), randomRow);
    enemy->setColliderHandle(m_collisionSystem.createCollider(Collider::Flag::Enemy, enemy.get()));
    m_enemies.push_back(std::move(enemy));
}

void EnemyManager::onEnemiesDestroyed(std::function<void(int)> callback)
{
    m_onEnemiesDestroyedCallback = callback;
}

void EnemyManager::notifyEnemiesDestroyed(int numberOfDestroyedEnemies)
{
    m_onEnemiesDestroyedCallback(numberOfDestroyedEnemies);
}

