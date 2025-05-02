#pragma once
#include "IGameState.h"
#include "enemy/EnemyManager.h"
#include "defender/DefenderManager.h"
#include "bullet/BulletManager.h"
#include "collisions/CollisionSystem.h"
#include "GUI/HUD.h"

class Game;

struct Wall
{
	ColliderHandle colliderHandle;
};

class PlayState : public IGameState
{
public:
	PlayState(Game& game);
	~PlayState();

	void update(Game& game, float dt) override;
	void draw(Game& game) override;

	void onEnter(Game& game) override;
	void onExit(Game& game) override;

private:
	void drawGrid();
	void goToWinState(Game& game);
	void updateBatteryAndScraps(float scrapGain, float batteryDrain);
	void performDefenderSpawnOnInput();
	void performActions(const Actions& actions);
	void performAction(const BulletSpawnAction& action);
	bool canAffordCost(int cost) const;
	bool canPlaceDefender(int x, int y) const;
	void setupHUD();
	void togglePause();

	void manageCollision(const Collision& collision);
	void manageBulletEnemyCollision(const Collision& collision);
	void manageDefenderEnemyCollision(const Collision& collision);
	void manageBaseWallEnemyCollision(const Collision& collision);

	int m_numberOfDestroyedEnemies{0};
	int m_numberOfEnemiesToKill{2000};

	float m_batteryCharge{ MAX_BATTERY_CHARGE };
	float m_scraps{};

	Game& m_game;
	CollisionSystem m_collisionSystem;
	DefenderManager m_defenderManager;
	EnemyManager m_enemyManager;
	BulletManager m_bulletManager;
	HUD m_hud;
	bool m_isGamePaused{};

	Wall m_baseWall;
};