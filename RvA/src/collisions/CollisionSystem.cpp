#include "collisions/CollisionSystem.h"

#include "defender/DefenderManager.h"
#include "enemy/EnemyManager.h"
#include "bullet/BulletManager.h"

CollisionSystem::CollisionSystem(const DefenderManager& defenderManager, const EnemyManager& enemyManager, const BulletManager& bulletManager)
	: m_defenderManager(defenderManager), m_enemyManager(enemyManager), m_bulletManager(bulletManager)
{
}

void CollisionSystem::enableDebugView(bool enabled)
{
	m_enableDebugView = enabled;
}

void CollisionSystem::toggleDebugView()
{
	m_enableDebugView = !m_enableDebugView;
}

void CollisionSystem::draw()
{
	if(m_enableDebugView)
	{
		for (const auto& defender : m_defenderManager.getDefenders())
		{
			// the defender doesn't have, yet, a bounding box so we're going to calculate it
			auto boundingBox = Rectangle{ defender->position.x, defender->position.y, 32, 32 };
			DrawRectangleLinesEx(boundingBox, 2, YELLOW);
		}

		for (const auto& bullet : m_bulletManager.getBullets())
		{
			DrawRectangleLinesEx(bullet->boundingBox, 2, YELLOW);
		}

		for (const auto& enemy : m_enemyManager.getEnemies())
		{
			DrawRectangleLinesEx(enemy->getBoundingBox(), 2, YELLOW);
		}
	}
}
