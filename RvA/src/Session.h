#pragma once

#include "Config.h"
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

class Config;
class GUI;
class MusicManager;

struct Wall {
	ColliderHandle colliderHandle;
};

enum class SessionState {
	Idle,
	Win,
	End,
	Skip,
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
	Session(GUI& gui, ResourceSystem& resourceSystem, const GameRegistry& gameRegistry, Config& config, MusicManager& musicManager);
	~Session();

	bool isState(SessionState state) const;

	void update(float dt);
	void draw(Atlas& atlas);

	void resetProgression();
	void startCurrentLevel();
	void startNextLevel();
	void startLevel(const std::string& levelId);
	void pause();
	void resume();

	// Demo Mode is used to let the game run without player intervention.
	// There's no win/lose, bots spawn automatically and the game runs until the player start a real game.
	void setDemoMode(bool demoMode);

	void setSelectedDefender(const std::optional<std::string>& type);

	auto getGameState() const {
		return m_gameState;
	}

	const auto& getCurrentLevel() const {
		return *m_levelData;
	}

private:
	void drawGrid();
	void updateBatteryAndScraps(float batteryDrain);
	void updateEnabledDefendersStats();
	void performDefenderSpawnOnInput();

	void performActions(const GameActions& actions);
	void performAction(const GameAction& action);
	void performAction(const BulletSpawnAction& action);
	void performAction(const EnemySpawnAction& action);
	void performAction(const PortalSpawnAction& action);
	void performAction(const DefenderSpawnAction& action);
	void performAction(const WinAction& action);
	void performAction(const LoseAction& action);
	void performAction(const MessageAction& action);
	void performAction(const HUDAction& action);
	void performAction(const DefenderPickerAction& action);
	void performAction(const EnableDefenderAction& action);
	void performAction(const UpdateValidCellAction& action);
	void performAction(const EndAction& action);
	void performAction(const ChangeSpeed& action);
	void performAction(const KillRandomEnemy& action);
	void performAction(const AddBattery& action);
	void performAction(const std::monostate& action);

	template<typename T> void performAction(const T&) {
		static_assert(sizeof(T) == 0, "Missing performAction overload for this type");
	}

	bool canAffordCost(int cost) const;
	bool canPlaceDefender(int x, int y) const;

	void setState(SessionState state);
	void setupHUD();
	void updateHUD(float dt);
	void refreshHUDDefenderPickerData();

	void onEnemiesDestroyed(const std::vector<EnemyDestroyedInfo>& enemies);
	void onDefenderDestroyed(Defender& defender);
	void onDropCollected(const std::vector<CollectedDrop>& collectedDrops);

	void manageCollision(const Collision& collision);
	void manageBulletEnemyCollision(const Collision& collision);
	void manageDefenderEnemyCollision(const Collision& collision);
	void manageBaseWallEnemyCollision(const Collision& collision);
	void manageEnemyPortalCollision(const Collision& collision);

	void resetSelectedDefender();
	void clearAllEntities();
	void startLevel();
	void updateGameSpeed();

	// cheats functions
	void changeGameSpeed();
	void killRandomEnemy();
	void addBattery(int battery);

	SessionState m_gameState{SessionState::Idle};
	std::optional<std::string> m_selectedDefender;
	Wall m_baseWall;
	LevelData* m_levelData{};
	CallbackHandle m_onDefenderSelectedCallbackHandle;
	CallbackHandle m_onSkipCallbackHandle;
	CallbackHandle m_onActionCallbackHandle;
	CallbackHandle m_onDefenderDestroyedHandle;
	CallbackHandle m_onEnemiesDestroyedHandle;
	CallbackHandle m_onCollectedDropHandle;
	CallbackHandle m_onLevelManagerActionHandle;

	const GameRegistry& m_gameRegistry;
	MusicManager& m_musicManager;
	Config& m_config;
	CollisionSystem m_collisionSystem;
	EnemyManager m_enemyManager;
	DefenderManager m_defenderManager;
	BulletManager m_bulletManager;
	DropManager m_dropManager;
	DefenderPicker m_defenderPicker;
	LevelManager m_levelManager;
	PortalManager m_portalManager;
	HUD m_hud;
	bool m_demoMode{};
	bool m_pauseGameplayLogic{};
	float m_gridDrawTime{};
	Interpolation<float> m_gameSpeedInterpolation;
};
