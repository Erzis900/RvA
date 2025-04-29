#pragma once
#include "IGameState.h"
#include "GUI/GUI.h"
#include "enemy/EnemyManager.h"
#include "defender/DefenderManager.h"

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

	int m_rows{ ROWS };
	int m_cols{ COLS };
	int m_numberOfDestroyedEnemies{0};
	int m_numberOfEnemiesToKill{5};

	float m_energy{ MAX_ENERGY };

	EnemyManager m_enemyManager;
	DefenderManager m_defenderManager;
};