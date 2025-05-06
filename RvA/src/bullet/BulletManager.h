#pragma once

#include <bullet/BulletTypes.h>
#include <memory>
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <vector>

class CollisionSystem;
class EnemyManager;
class Enemy;

class BulletTypeRegistry {
public:
	void registerBulletType(std::string id, BulletData typeInfo);
	const BulletData* getBulletInfo(const std::string& id) const;

private:
	std::unordered_map<std::string, BulletData> m_bulletTypes;
};

class BulletManager {
public:
	BulletManager(EnemyManager& enemyManager, CollisionSystem& collisionSystem);

	void clear();
	void update(float dt);
	void draw();

	void spawnBullet(const BulletData& data, const Vector2& position);
	void executeHit(Bullet& bullet, Enemy& enemy);

	auto& getBullets() const {
		return m_bullets;
	}

private:
	// TODO(Gerark): These methods likely belong within the Info structs for better encapsulation.
	// We'll monitor how the codebase evolves and consider refactoring toward a more OOP approach if it proves beneficial.
	void setupBullet(Bullet& bullet, BulletShotData& data);
	void updateBullet(Bullet& bullet, BulletShotData& data, float dt);
	void drawBullet(Bullet& bullet, BulletShotData& data);
	void onEnemyHit(Enemy& enemy, Bullet& bullet, BulletShotData& data, float dt);

	void setupBullet(Bullet& bullet, LaserBeamData& data);
	void updateBullet(Bullet& bullet, LaserBeamData& data, float dt);
	void drawBullet(Bullet& bullet, LaserBeamData& data);
	void onEnemyHit(Enemy& enemy, Bullet& bullet, LaserBeamData& data, float dt);

	void setupBullet(Bullet& bullet, ChasingShotData& data);
	void updateBullet(Bullet& bullet, ChasingShotData& data, float dt);
	void drawBullet(Bullet& bullet, ChasingShotData& data);
	void onEnemyHit(Enemy& enemy, Bullet& bullet, ChasingShotData& data, float dt);

	// TODO(Gerark) A vector of unique_ptr isn't really the best choice from a memory layout point of view. It can be improved.
	std::vector<std::unique_ptr<Bullet>> m_bullets;
	EnemyManager& m_enemyManager;
	CollisionSystem& m_collisionSystem;
	float m_latestDeltaTime{};
};
