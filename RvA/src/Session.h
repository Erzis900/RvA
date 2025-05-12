#pragma once

#include "GUI/FadeScreen.h"
#include "GUI/HUD.h"
#include "bullet/BulletManager.h"
#include "collisions/Collider.h"
#include "collisions/CollisionSystem.h"
#include "defender/DefenderManager.h"
#include "defender/DefenderPicker.h"
#include "drops/DropManager.h"
#include "enemy/EnemyManager.h"
#include "levels/LevelManager.h"
#include "portal/PortalManager.h"

class Game;
class GUI;

struct Wall {
	ColliderHandle colliderHandle;
};

enum class SessionState {
	Idle,
	Win,
	End,
	Lost,
	Paused,
	Playing
};

/*
 * A Session class represent a single run of a Level.
 * Running a sequence of levels will require a reset of the session ( start/end ) for each level started.
 */
class Session {
public:
	Session(GUI& gui, const GameRegistry& gameRegistry);
	~Session();

	void setState(SessionState state);
	bool isPaused() const;

	void update(float dt);
	void draw(Atlas& atlas);

	// Demo Mode is used to let the game run without player intervention.
	// There's no win/lose, bots spawn automatically and the game runs until the player start a real game.
	void setDemoMode(bool demoMode);

	void setSelectedDefender(const std::optional<std::string>& type);

	auto getGameState() const {
		return m_gameState;
	}

	auto& getCollisionSystem() {
		return m_collisionSystem;
	}

	const auto& getDefenderManager() const {
		return m_defenderManager;
	}

	const auto& getEnemyManager() const {
		return m_enemyManager;
	}

	const auto& getCurrentLevel() const {
		return *m_levelData;
	}

private:
	void drawGrid();
	void updateBatteryAndScraps(float scrapGain, float batteryDrain);
	void performDefenderSpawnOnInput();

	void performActions(const GameActions& actions);
	void performAction(const GameAction& action);
	void performAction(const BulletSpawnAction& action);
	void performAction(const EnemySpawnAction& action);
	void performAction(const PortalSpawnAction& action);
	void performAction(const DefenderSpawnAction& action);
	void performAction(const WinAction& action);
	void performAction(const LoseAction& action);

	template<typename T> void performAction(const T&) {
		static_assert(sizeof(T) == 0, "Missing performAction overload for this type");
	}

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
	void manageEnemyPortalCollision(const Collision& collision);

	void resetSelectedDefender();
	void clearAllEntities();

	SessionState m_gameState{SessionState::Idle};
	std::optional<std::string> m_selectedDefender;
	Wall m_baseWall;
	LevelData* m_levelData{};
	CallbackHandle m_onDefenderSelectedCallbackHandle;
	CallbackHandle m_onDefenderDestroyedHandle;
	CallbackHandle m_onEnemiesDestroyedHandle;
	CallbackHandle m_onCollectedDropHandle;
	CallbackHandle m_onLevelManagerActionHandle;

	const GameRegistry& m_gameRegistry;
	CollisionSystem m_collisionSystem;
	DefenderManager m_defenderManager;
	EnemyManager m_enemyManager;
	BulletManager m_bulletManager;
	DropManager m_dropManager;
	DefenderPicker m_defenderPicker;
	LevelManager m_levelManager;
	PortalManager m_portalManager;
	HUD m_hud;
	bool m_demoMode{false};
};
