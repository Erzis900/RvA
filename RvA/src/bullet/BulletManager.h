#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>
#include <raylib.h>
#include <bullet/BulletTypes.h>

class EnemyManager;
class Enemy;

class BulletTypeRegistry
{
public:
	void registerBulletType(std::string id, BulletData typeInfo);
	const BulletData* getBulletInfo(const std::string& id) const;

private:
	std::unordered_map<std::string, BulletData> m_bulletTypes;
};

class BulletManager
{
public:
	BulletManager(EnemyManager& enemyManager);

	void update(float dt);
	void draw();

	void spawnBullet(const BulletData& data, const Vector2& position);

private:
	void manageEnemyCollisions(Bullet2& bullet);

	// TODO(Gerark): These methods likely belong within the Info structs for better encapsulation.
	// We'll monitor how the codebase evolves and consider refactoring toward a more OOP approach if it proves beneficial.
	void setupBullet(Bullet2& bullet, BulletShotData& data);
	void updateBullet(Bullet2& bullet, BulletShotData& data, float dt);
	void drawBullet(Bullet2& bullet, BulletShotData& data);
	void onEnemyHit(Enemy& enemy, Bullet2& bullet, BulletShotData& data);

	void setupBullet(Bullet2& bullet, LaserBeamData& data);
	void updateBullet(Bullet2& bullet, LaserBeamData& data, float dt);
	void drawBullet(Bullet2& bullet, LaserBeamData& data);
	void onEnemyHit(Enemy& enemy, Bullet2& bullet, LaserBeamData& data);

	// TODO(Gerark) A vector of unique_ptr isn't really the best choice from a memory layout point of view. It can be improved.
	std::vector<std::unique_ptr<Bullet2>> m_bullets;
	EnemyManager& m_enemyManager;
};
