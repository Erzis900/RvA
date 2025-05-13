#include "bullet/BulletManager.h"

#include "collisions/CollisionSystem.h"
#include "constants.h"
#include "enemy/EnemyManager.h"

#include <raymath.h>
#include <reasings.h>

BulletManager::BulletManager(EnemyManager& enemyManager, CollisionSystem& collisionSystem, MusicManager& musicManager) : m_enemyManager(enemyManager), m_collisionSystem(collisionSystem), m_musicManager(musicManager) {
	m_bullets.reserve(128);
}

void BulletManager::clear() {
	for (auto& bullet : m_bullets) {
		m_collisionSystem.destroyCollider(bullet->colliderHandle);
	}
	m_bullets.clear();
	m_latestDeltaTime = 0;
}

void BulletManager::update(float dt) {
	m_latestDeltaTime = dt;
	for (auto it = m_bullets.begin(); it != m_bullets.end();) {
		auto& bullet = *it;

		std::visit([this, &bullet, dt](auto&& data) { updateBullet(*bullet, data, dt); }, bullet->data);

		bullet->lifetime -= dt;
		if (bullet->lifetime <= 0) {
			m_collisionSystem.destroyCollider((*it)->colliderHandle);
			it = m_bullets.erase(it);
		} else {
			++it;
		}
	}
}

void BulletManager::draw() {
	for (auto& bullet : m_bullets) {
		std::visit([this, &bullet](auto&& data) { drawBullet(*bullet, data); }, bullet->data);
	}
}

void BulletManager::spawnBullet(const BulletData& data, const Vector2& position) {
	auto bullet = std::make_unique<Bullet>();
	bullet->data = data;
	bullet->position = position;
	bullet->colliderHandle = m_collisionSystem.createCollider(Collider::Flag::Bullet, bullet.get());
	std::visit([this, &bullet](auto&& data) { setupBullet(*bullet, data); }, bullet->data);
	m_bullets.push_back(std::move(bullet));
}

void BulletManager::executeHit(Bullet& bullet, Enemy& enemy) {
	std::visit([this, &enemy, &bullet](auto&& data) { onEnemyHit(enemy, bullet, data, m_latestDeltaTime); }, bullet.data);
}

/*
 * Simple Bullet
 */
void BulletManager::setupBullet(Bullet& bullet, BulletShotData& data) {
	bullet.lifetime = data.maxLifetime;
	bullet.position = Vector2Add(bullet.position, data.offsetPos);

	auto side = data.radius * 2;
	m_collisionSystem.updateCollider(bullet.colliderHandle, {bullet.position.x - data.radius, bullet.position.y - data.radius, side, side});
}

void BulletManager::updateBullet(Bullet& bullet, BulletShotData& data, float dt) {
	data.time += dt;
	bullet.position = Vector2Add(bullet.position, Vector2Scale(data.velocity, dt));
	m_collisionSystem.updateColliderPosition(bullet.colliderHandle, {bullet.position.x - data.radius, bullet.position.y - data.radius});
}

void BulletManager::drawBullet(Bullet& bullet, BulletShotData& data) {
	DrawEllipse(bullet.position.x, bullet.position.y + 23, data.radius * 1.5f, data.radius * 1.5f, Fade(BLACK, 0.1f));

	float max = data.radius * 1.5f;
	float min = data.radius;
	float amplitude = (max - min) / 2.0f;
	float mid = (max + min) / 2.0f;
	float finalRadius = mid + amplitude * std::sin(data.time * 10);

	DrawCircleV(bullet.position, finalRadius, WHITE);
}

void BulletManager::onEnemyHit(Enemy& enemy, Bullet& bullet, BulletShotData& data, float dt) {
	bullet.lifetime = 0;
	enemy.applyDamage(data.damage.createDamage(dt));
}

/*
 * Laser Beam
 */
void BulletManager::setupBullet(Bullet& bullet, LaserBeamData& data) {
	bullet.lifetime = data.maxLifetime;
	bullet.position = Vector2Add(bullet.position, data.startOffset);
	data.beamWidth = 0;
	m_collisionSystem.updateCollider(bullet.colliderHandle, {bullet.position.x, bullet.position.y - data.beamHeight / 2, data.beamWidth, data.beamHeight});
	m_musicManager.play(m_musicManager.getLaserShoot());
}

void BulletManager::updateBullet(Bullet& bullet, LaserBeamData& data, float dt) {
	data.shootAnimationTime += dt * data.shootAnimationSpeed;

	float currentValue = Clamp(data.shootAnimationTime, 0, data.shootAnimationDuration);
	float value = EaseSineIn(currentValue, 0, 1, data.shootAnimationDuration);
	data.beamWidth = (GAME_RENDERTEXTURE_SIZE.x - CELL_SIZE - bullet.position.x) * value;

	m_collisionSystem.updateCollider(bullet.colliderHandle, {bullet.position.x, bullet.position.y - data.beamHeight / 2, data.beamWidth, data.beamHeight});
}

void BulletManager::drawBullet(Bullet& bullet, LaserBeamData& data) {
	float width = data.beamWidth;
	float height = data.beamHeight * (1 - ((data.shootAnimationTime - 1) / data.shootAnimationDuration));
	float halfHeight = height / 2;

	DrawRectangleGradientEx({bullet.position.x, bullet.position.y - halfHeight, width, height}, data.beamStartColor, data.beamStartColor, data.beamEndColor, data.beamEndColor);
	DrawRectangleGradientV(bullet.position.x, bullet.position.y - halfHeight - height + 2, width, height, Fade(data.beamEndColor, 0), data.beamStartColor);
	DrawRectangleGradientV(bullet.position.x, bullet.position.y - halfHeight + height, width, height, data.beamStartColor, Fade(data.beamEndColor, 0));
}

void BulletManager::onEnemyHit(Enemy& enemy, Bullet& bullet, LaserBeamData& data, float dt) {
	enemy.applyDamage(data.damage.createDamage(dt));
}

/*
 * Chasing Bullet
 */
void BulletManager::setupBullet(Bullet& bullet, ChasingShotData& data) {
	bullet.lifetime = data.maxLifetime;
	bullet.position = Vector2Add(bullet.position, data.startOffset);
	auto halfRadius = data.radius * 0.5f;
	m_collisionSystem.updateCollider(bullet.colliderHandle, {bullet.position.x - halfRadius, bullet.position.y - halfRadius, data.radius, data.radius});
}

void BulletManager::updateBullet(Bullet& bullet, ChasingShotData& data, float dt) {
	auto enemy = m_enemyManager.findClosestEnemy(bullet.position, true);
	if (enemy) {
		auto targetDir = Vector2Normalize(Vector2Subtract(enemy->getCenteredPosition(), bullet.position));
		data.direction = Vector2Lerp(data.direction, targetDir, 0.1f);
		if (Vector2Length(data.direction) > data.speed) {
			data.direction = Vector2Normalize(data.direction);
		}
	}

	bullet.position = Vector2Add(bullet.position, Vector2Scale(data.direction, dt * data.speed));
	m_collisionSystem.updateColliderPosition(bullet.colliderHandle, {bullet.position.x - data.radius * 0.5f, bullet.position.y - data.radius * 0.5f});
}

void BulletManager::drawBullet(Bullet& bullet, ChasingShotData& data) {
	Vector2 forward = Vector2Normalize(data.direction);
	Vector2 right = {-forward.y, forward.x}; // perpendicular to forward

	Vector2 tip = Vector2Add(bullet.position, Vector2Scale(forward, data.radius * 0.5f));
	Vector2 baseLeft = Vector2Add(bullet.position, Vector2Scale(right, data.radius * 0.5f));
	Vector2 baseRight = Vector2Subtract(bullet.position, Vector2Scale(right, data.radius * 0.5f));

	DrawTriangle(baseRight, baseLeft, tip, data.color);
	DrawTriangle(Vector2Add(baseRight, {0, 23}), Vector2Add(baseLeft, {0, 23}), Vector2Add(tip, {0, 23}), Fade(BLACK, 0.1f));
}

void BulletManager::onEnemyHit(Enemy& enemy, Bullet& bullet, ChasingShotData& data, float dt) {
	bullet.lifetime = 0;
	enemy.applyDamage(data.damage.createDamage(dt));
}
