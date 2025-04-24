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

	int m_rows;
	int m_cols;
	int m_cellSize;

	float m_energy;

	EnemyManager m_enemyManager;
	DefenderManager m_defenderManager;
};