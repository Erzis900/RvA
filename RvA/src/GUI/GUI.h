#pragma once

class Game;

class GUI
{
public:
	GUI(Game& game);
	void drawGame(int cellSize, int rows, float energy);
	void drawMenu();
	void drawCursor();
private:
	void drawEnergyBar(int cellSize, int rows, float energy);
	void drawDefenders();

	Game& m_game;
};