#pragma once

#include "collisions/Collider.h"
#include "enemy/EnemyManager.h"
#include "defender/DefenderManager.h"
#include "bullet/BulletManager.h"
#include "collisions/CollisionSystem.h"

class Game;

struct Wall
{
	ColliderHandle colliderHandle;
};

class Session
{
public:
	Session(Game& game, CollisionSystem& collisionSystem);
	~Session();

	void update(Game& game, float dt);
	void draw(Game& game);

	int getNumberOfDestroyedEnemies() const;
	float getBatteryCharge() const;
	float getScraps() const;
	void setSelectedDefender(std::optional<DefenderType> type);

private:
	void drawGrid();
	void updateBatteryAndScraps(float scrapGain, float batteryDrain);
	void performDefenderSpawnOnInput();
	void performActions(const Actions& actions);
	void performAction(const BulletSpawnAction& action);
	bool canAffordCost(int cost) const;
	bool canPlaceDefender(int x, int y) const;

	void manageCollision(const Collision& collision);
	void manageBulletEnemyCollision(const Collision& collision);
	void manageDefenderEnemyCollision(const Collision& collision);
	void manageBaseWallEnemyCollision(const Collision& collision);

	int m_numberOfDestroyedEnemies{ 0 };

	float m_batteryCharge{ MAX_BATTERY_CHARGE };
	float m_scraps{};

	Game& m_game;
	CollisionSystem& m_collisionSystem;
	DefenderManager m_defenderManager;
	EnemyManager m_enemyManager;
	BulletManager m_bulletManager;
	std::optional<DefenderType> m_selectedDefender;

	Wall m_baseWall;
};