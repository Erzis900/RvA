#pragma once
#include "defender/DefenderTypes.h"
#include <raylib.h>

class Game;

class GUI
{
public:
	GUI(Game& game);
	void drawGame(int cellSize, int rows, float energy);
	void drawMenu();
	void drawCursor();
	void drawHp(int cellSize, int hp, int maxHp, Vector2 pos);

	DefenderType getSelectedDefender() { return m_selectedDefender; }
private:
	void drawEnergyBar(int cellSize, int rows, float energy);
	void drawDefenders(int cellSize);

	Game& m_game;
	DefenderType m_selectedDefender;
};