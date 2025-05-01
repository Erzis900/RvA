#include "bullet/BulletManager.h"
#include "enemy/EnemyManager.h"
#include <raymath.h>
#include <reasings.h>
#include <constants.h>

BulletManager::BulletManager(EnemyManager& enemyManager) : m_enemyManager(enemyManager)
{
	m_bullets.reserve(128);
}

void BulletTypeRegistry::registerBulletType(std::string id, BulletData data)
{
	m_bulletTypes.insert({ std::move(id), std::move(data) });
}

const BulletData* BulletTypeRegistry::getBulletInfo(const std::string& id) const
{
	if (auto itr = m_bulletTypes.find(id); itr != m_bulletTypes.end())
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

		std::visit([this, &bullet, dt](auto&& data) { updateBullet(*bullet, data, dt); }, bullet->data);

		manageEnemyCollisions(*bullet, dt);

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
		std::visit([this, &bullet](auto&& data) { drawBullet(*bullet, data); }, bullet->data);
	}
}

void BulletManager::spawnBullet(const BulletData& data, const Vector2& position)
{
	auto bullet = std::make_unique<Bullet2>();
	bullet->data = data;
	bullet->position = position;
	std::visit([this, &bullet](auto&& data) { setupBullet(*bullet, data); }, bullet->data);
	m_bullets.push_back(std::move(bullet));
}

void BulletManager::manageEnemyCollisions(Bullet2& bullet, float dt)
{
	// TODO(Gerark): It might be worth separating bullet/enemy collision logic into a more generic collision system.
	// Currently, each bullet has a bounding box, and we check for collisions directly against the enemy's bounding box.
	for (auto& enemy : m_enemyManager.getEnemies())
	{
		if (!enemy->isDying())
		{
			// For now, we construct the enemy's bounding box on the fly.
			auto enemyBoundingBox = enemy->getBoundingBox();
			if (CheckCollisionRecs(bullet.boundingBox, enemyBoundingBox))
			{
				std::visit([this, &enemy, &bullet, dt](auto&& data) { onEnemyHit(*enemy, bullet, data, dt); }, bullet.data);
			}
		}
	}
}

/*
* Simple Bullet
*/
void BulletManager::setupBullet(Bullet2& bullet, BulletShotData& data)
{
	bullet.lifetime = data.maxLifetime;
	bullet.position = Vector2Add(bullet.position, { 20, 20 });
	
	auto side = data.radius * 2;
	bullet.boundingBox = { bullet.position.x - data.radius, bullet.position.y - data.radius, side, side };
}

void BulletManager::updateBullet(Bullet2& bullet, BulletShotData& data, float dt)
{
	bullet.position = Vector2Add(bullet.position, Vector2Scale(data.velocity, dt));
	bullet.boundingBox.x = bullet.position.x - data.radius;
	bullet.boundingBox.y = bullet.position.y - data.radius;
}

void BulletManager::drawBullet(Bullet2& bullet, BulletShotData& data)
{
	DrawCircleV(bullet.position, data.radius, BLUE);
}

void BulletManager::onEnemyHit(Enemy& enemy, Bullet2& bullet, BulletShotData& data, float dt)
{
	bullet.lifetime = 0;
	enemy.takeDamage(data.damage);
}

/*
* Laser Beam
*/
void BulletManager::setupBullet(Bullet2& bullet, LaserBeamData& data)
{
	bullet.lifetime = data.maxLifetime;
	bullet.position = Vector2Add(bullet.position, data.startOffset);
	data.beamWidth = 0;
	bullet.boundingBox = { bullet.position.x, bullet.position.y - data.beamHeight / 2, data.beamWidth, data.beamHeight };
}

void BulletManager::updateBullet(Bullet2& bullet, LaserBeamData& data, float dt)
{
	data.shootAnimationTime += dt * data.shootAnimationSpeed;

	float currentValue = Clamp(data.shootAnimationTime, 0, data.shootAnimationDuration);
	float value = EaseSineIn(currentValue, 0, 1, data.shootAnimationDuration);
	data.beamWidth = (TEX_WIDTH - CELL_SIZE - bullet.position.x) * value;

	bullet.boundingBox = { bullet.position.x, bullet.position.y - data.beamHeight / 2, data.beamWidth, data.beamHeight };
}

void BulletManager::drawBullet(Bullet2& bullet, LaserBeamData& data)
{
	float width = data.beamWidth;
	float height = data.beamHeight;
	float halfHeight = height / 2;

	float shake = sinf(data.shootAnimationTime * 40.0f) * 2.0f;

	DrawRectangle(
		bullet.position.x + halfHeight,
		bullet.position.y - halfHeight - data.auraSize / 2,
		width,
		height + data.auraSize,
		data.auraColor
	);
	DrawCircleV({ bullet.position.x + width, bullet.position.y }, height, data.auraColor);
	DrawRectangle(bullet.position.x, bullet.position.y - halfHeight + shake, width, height, data.beamColor);

	DrawCircleV(bullet.position, height, data.beamColor);
	DrawCircleV({ bullet.position.x + width, bullet.position.y + shake }, height * 0.5f, data.beamColor);
}

void BulletManager::onEnemyHit(Enemy& enemy, Bullet2& bullet, LaserBeamData& data, float dt)
{
	enemy.takeDamage(data.damage * dt);
}

/*
* Chasing Bullet
*/
void BulletManager::setupBullet(Bullet2& bullet, ChasingShotData& data)
{
	bullet.lifetime = data.maxLifetime;
	bullet.position = Vector2Add(bullet.position, { 20, 20 });
	auto halfRadius = data.radius * 0.5f;
	bullet.boundingBox = { bullet.position.x - halfRadius, bullet.position.y - halfRadius, data.radius, data.radius };
}

void BulletManager::updateBullet(Bullet2& bullet, ChasingShotData& data, float dt)
{
	auto enemy = m_enemyManager.findClosestEnemy(bullet.position, true);
	if (enemy)
	{
		auto targetDir = Vector2Normalize(Vector2Subtract(enemy->getCenteredPosition(), bullet.position));
		data.direction = Vector2Lerp(data.direction, targetDir, 0.1f);
		if (Vector2Length(data.direction) > data.speed)
		{
			data.direction = Vector2Normalize(data.direction);
		}
	}

	bullet.position = Vector2Add(bullet.position, Vector2Scale(data.direction, dt * data.speed));
	bullet.boundingBox.x = bullet.position.x - data.radius * 0.5f;
	bullet.boundingBox.y = bullet.position.y - data.radius * 0.5f;
}

void BulletManager::drawBullet(Bullet2& bullet, ChasingShotData& data)
{
	Vector2 forward = Vector2Normalize(data.direction);
	Vector2 right = { -forward.y, forward.x }; // perpendicular to forward

	Vector2 tip = Vector2Add(bullet.position, Vector2Scale(forward, data.radius * 0.5f));
	Vector2 baseLeft = Vector2Add(bullet.position, Vector2Scale(right, data.radius * 0.5f));
	Vector2 baseRight = Vector2Subtract(bullet.position, Vector2Scale(right, data.radius * 0.5f));

	DrawTriangle(baseRight, baseLeft, tip, data.color);
}

void BulletManager::onEnemyHit(Enemy& enemy, Bullet2& bullet, ChasingShotData& data, float dt)
{
	bullet.lifetime = 0;
	enemy.takeDamage(data.damage);
}
