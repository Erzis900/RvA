#include "bullet/BulletManager.h"
#include "enemy/EnemyManager.h"
#include <raymath.h>

BulletManager::BulletManager(EnemyManager& enemyManager) : m_enemyManager(enemyManager)
{
	m_bullets.reserve(128);
}

void BulletTypeRegistry::registerBulletType(BulletTypeInfo typeInfo)
{
	m_bulletTypes.insert({ typeInfo.type, std::move(typeInfo) });
}

const BulletTypeInfo* BulletTypeRegistry::getBulletInfo(BulletType type) const
{
	if (auto itr = m_bulletTypes.find(type); itr != m_bulletTypes.end())
	{
		return &itr->second;
	}

	return nullptr;
}

void BulletManager::update(float dt)
{
	for (auto it = m_bullets.begin(); it != m_bullets.end(); )
	{
		auto& bullet = *it;

		std::visit([this, &bullet, dt](auto&& behaviour) { updateBullet(*bullet, behaviour, dt); }, bullet->info->behaviour);

		manageEnemyCollisions(*bullet);

		bullet->lifetime -= dt;
		if (bullet->lifetime <= 0)
		{
			it = m_bullets.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void BulletManager::draw()
{
	for (auto& bullet : m_bullets)
	{
		std::visit([this, &bullet](auto&& behaviour) { drawBullet(*bullet, behaviour); }, bullet->info->behaviour);
	}
}

void BulletManager::spawnBullet(const BulletTypeInfo* info, const Vector2& position)
{
	auto bullet = std::make_unique<Bullet2>();
	bullet->info = info;
	bullet->position = position;
	bullet->lifetime = info->maxLifetime;
	std::visit([this, &bullet](auto&& behaviour) { setupBullet(*bullet, behaviour); }, info->behaviour);
	m_bullets.push_back(std::move(bullet));
}

void BulletManager::manageEnemyCollisions(Bullet2& bullet)
{
	// TODO(Gerark): It might be worth separating bullet/enemy collision logic into a more generic collision system.
	// Currently, each bullet has a bounding box, and we check for collisions directly against the enemy's bounding box.
	for (auto& enemy : m_enemyManager.getEnemies())
	{
		// For now, we construct the enemy's bounding box on the fly.
		auto& position = enemy->getPosition();
		auto enemyBoundingBox = Rectangle{ position.x, position.y, 32, 32 };
		if (CheckCollisionRecs(bullet.boundingBox, enemyBoundingBox))
		{
			std::visit([this, &enemy, &bullet](auto&& behaviour) { onEnemyHit(*enemy, bullet, behaviour); }, bullet.info->behaviour);
		}
	}
}

/*
* Simple Bullet
*/
void BulletManager::setupBullet(Bullet2& bullet, const BulletShotInfo& info)
{
	bullet.position = Vector2Add(bullet.position, { 20, 20 });
	bullet.boundingBox = { bullet.position.x, bullet.position.y, info.radius * 2, info.radius * 2 };
}

void BulletManager::updateBullet(Bullet2& bullet, const BulletShotInfo& info, float dt)
{
	bullet.position = Vector2Add(bullet.position, Vector2Scale(info.velocity, dt));
	bullet.boundingBox.x = bullet.position.x;
	bullet.boundingBox.y = bullet.position.y;
}

void BulletManager::drawBullet(Bullet2& bullet, const BulletShotInfo& info)
{
	DrawCircleV(bullet.position, info.radius, BLUE);
}

void BulletManager::onEnemyHit(Enemy& enemy, Bullet2& bullet, const BulletShotInfo& info)
{
	bullet.lifetime = 0;
	enemy.takeDamage(static_cast<int>(info.damage));
}

/*
* Laser Beam
*/
void BulletManager::setupBullet(Bullet2& bullet, const LaserBeamInfo& info)
{
	bullet.position = Vector2Add(bullet.position, info.startOffset);
	bullet.boundingBox = { bullet.position.x, bullet.position.y - info.laserHeight / 2, 200, info.laserHeight };
}

void BulletManager::updateBullet(Bullet2& bullet, const LaserBeamInfo& info, float dt)
{
}

void BulletManager::drawBullet(Bullet2& bullet, const LaserBeamInfo& info)
{
	float width = 400;
	static float time = 0;
	time += GetFrameTime();
	float shake = sinf(time * 60.0f) * 2.0f;
	DrawRectangle(bullet.position.x + info.laserHeight / 2, bullet.position.y - info.laserHeight / 2 - info.auraSize / 2 + shake, width, info.laserHeight + info.auraSize, info.auraColor);
	DrawRectangle(bullet.position.x, bullet.position.y - info.laserHeight / 2 + shake, width, info.laserHeight, info.beamColor);

	DrawCircleV(bullet.position, info.laserHeight, BLUE);
	DrawCircleV({ bullet.position.x + width, bullet.position.y + shake }, info.laserHeight * 0.5f, info.beamColor);
}

void BulletManager::onEnemyHit(Enemy& enemy, Bullet2& bullet, const LaserBeamInfo& info)
{
	enemy.takeDamage(static_cast<int>(info.damage));
}
