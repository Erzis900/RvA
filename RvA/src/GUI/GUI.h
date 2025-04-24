#pragma once
#include "defender/DefenderTypes.h"
#include <raylib.h>

class Game;
class DefenderManager;

class GUI
{
public:
	GUI(Game& game);
	void drawGame(int cellSize, int rows, float energy, DefenderManager& defenderManager);
	void drawCursor();
	void drawHp(int cellSize, int hp, int maxHp, Vector2 pos);

	DefenderType getSelectedDefender() const { return m_selectedDefender; }
	int& getBatteries() { return batteries; }
	bool isPaused() const { return m_paused; }
private:
	void drawEnergyBar(int cellSize, int rows, float energy);
	void drawDefenders(int cellSize);
	void drawCosts(int cellSize, DefenderManager& defenderManager);

	int batteries;
	bool m_paused;

	Game& m_game;
	DefenderType m_selectedDefender;
};