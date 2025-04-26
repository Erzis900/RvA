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
	int& getBatteries() { return m_batteries; }
	bool isPaused() const { return m_paused; }

	void reset();
private:
	void drawEnergyBar(int cellSize, int rows, float energy);
	void drawDefenders(int cellSize);
	void drawCosts(int cellSize, DefenderManager& defenderManager);

	int m_batteries;
	bool m_paused;
	bool m_defenderHover;

	Game& m_game;
	DefenderType m_selectedDefender;
};