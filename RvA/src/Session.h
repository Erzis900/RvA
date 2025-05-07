#pragma once

#include "GUI/HUD.h"
#include "bullet/BulletManager.h"
#include "collisions/Collider.h"
#include "collisions/CollisionSystem.h"
#include "defender/DefenderManager.h"
#include "defender/DefenderPicker.h"
#include "drops/DropManager.h"
#include "enemy/EnemyManager.h"

class Game;
class GUI;

struct Wall {
	ColliderHandle colliderHandle;
};

class Session {
public:
	Session(GUI& gui, const GameRegistry& gameRegistry);
	~Session();

	void start();
	void end();

	void setPause(bool paused);
	bool isPaused() const;

	void update(float dt);
	void draw(Atlas& atlas);

	auto getNumberOfDestroyedEnemies() const {
		return m_numberOfDestroyedEnemies;
	}

	auto getBatteryCharge() const {
		return m_batteryCharge;
	}

	auto getScraps() const {
		return m_scraps;
	}

	void setSelectedDefender(std::optional<DefenderType> type);

	auto& getCollisionSystem() {
		return m_collisionSystem;
	}

	const auto& getDefenderManager() const {
		return m_defenderManager;
	}

	const auto& getEnemyManager() const {
		return m_enemyManager;
	}

private:
	void drawGrid();
	void updateBatteryAndScraps(float scrapGain, float batteryDrain);
	void performDefenderSpawnOnInput();
	void performActions(const Actions& actions);
	void performAction(const BulletSpawnAction& action);
	bool canAffordCost(int cost) const;
	bool canPlaceDefender(int x, int y) const;

	void setupHUD();
	void updateHUD(float dt);

	void onEnemiesDestroyed(const std::vector<EnemyDestroyedInfo>& enemies);
	void onDropCollected(const std::vector<CollectedDrop>& collectedDrops);

	void manageCollision(const Collision& collision);
	void manageBulletEnemyCollision(const Collision& collision);
	void manageDefenderEnemyCollision(const Collision& collision);
	void manageBaseWallEnemyCollision(const Collision& collision);

	void resetSelectedDefender();

	int m_numberOfDestroyedEnemies{0};
	float m_batteryCharge{MAX_BATTERY_CHARGE};
	float m_scraps{};
	std::optional<DefenderType> m_selectedDefender;
	Wall m_baseWall;
	bool m_isPaused{false};
	bool m_isStarted{false};
	CallbackHandle m_onDefenderSelectedCallbackHandle;
	CallbackHandle m_onDefenderDestroyedHandle;
	CallbackHandle m_onEnemiesDestroyedHandle;
	CallbackHandle m_onCollectedDropHandle;

	const GameRegistry& m_gameRegistry;
	CollisionSystem m_collisionSystem;
	DefenderManager m_defenderManager;
	EnemyManager m_enemyManager;
	BulletManager m_bulletManager;
	DropManager m_dropManager;
	DefenderPicker m_defenderPicker;
	HUD m_hud;
};
