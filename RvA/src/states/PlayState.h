#pragma once
#include "IGameState.h"
#include "enemy/EnemyManager.h"
#include "defender/DefenderManager.h"
#include "bullet/BulletManager.h"

class Game;

class PlayState : public IGameState
{
public:
	PlayState(Game& game);

	void update(Game& game, float dt) override;
	void draw(Game& game) override;

private:
	void drawGrid();
	void goToWinState(Game& game);
	void updateEnergyAndBatteries(float batteryGain, float energyDrain);
	void performDefenderSpawnOnInput();
	void performActions(const Actions& actions);
	void performAction(const BulletSpawnAction& action);
	bool canAffordCost(int cost) const;
	bool canPlaceDefender(int x, int y) const;

	int m_rows{ ROWS };
	int m_cols{ COLS };
	int m_numberOfDestroyedEnemies{0};
	int m_numberOfEnemiesToKill{5};

	float m_energy{ MAX_ENERGY };
	float m_batteries{};

	Game& m_game;
	EnemyManager m_enemyManager;
	DefenderManager m_defenderManager;
	DefenderManager2 m_defenderManager2;
	BulletManager m_bulletManager;
};