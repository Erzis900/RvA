#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <variant>
#include <raylib.h>
#include <bullet/BulletTypes.h>

class EnemyManager;
class Enemy;

struct BulletShotInfo
{
	Vector2 velocity;
	float radius{};
	float damage{};
};

struct LaserBeamInfo
{

};

struct BulletTypeInfo
{
	BulletType type{};
	float maxLifetime{};
	std::variant<BulletShotInfo, LaserBeamInfo> behaviour;
};

struct Bullet2
{
	const BulletTypeInfo* info;
	Vector2 position;
	Rectangle boundingBox;
	float lifetime;
};

class BulletTypeRegistry
{
public:
	void registerBulletType(BulletTypeInfo typeInfo);
	const BulletTypeInfo* getBulletInfo(BulletType type) const;

private:
	std::unordered_map<BulletType, BulletTypeInfo> m_bulletTypes;
};

class BulletManager
{
public:
	BulletManager(EnemyManager& enemyManager);

	void update(float dt);
	void draw();

	void spawnBullet(const BulletTypeInfo* info, const Vector2& position);

private:
	void manageEnemyCollisions(Bullet2& bullet);

	// TODO(Gerark): These methods likely belong within the Info structs for better encapsulation.
	// We'll monitor how the codebase evolves and consider refactoring toward a more OOP approach if it proves beneficial.
	void setupBullet(Bullet2& bullet, const BulletShotInfo& info);
	void updateBullet(Bullet2& bullet, const BulletShotInfo& info, float dt);
	void drawBullet(Bullet2& bullet, const BulletShotInfo& info);
	void onEnemyHit(Enemy& enemy, Bullet2& bullet, const BulletShotInfo& info);

	void setupBullet(Bullet2& bullet, const LaserBeamInfo& info);
	void updateBullet(Bullet2& bullet, const LaserBeamInfo& info, float dt);
	void drawBullet(Bullet2& bullet, const LaserBeamInfo& info);
	void onEnemyHit(Enemy& enemy, Bullet2& bullet, const LaserBeamInfo& info);

	// TODO(Gerark) A vector of unique_ptr isn't really the best choice from a memory layout point of view. It can be improved.
	std::vector<std::unique_ptr<Bullet2>> m_bullets;
	EnemyManager& m_enemyManager;
};
