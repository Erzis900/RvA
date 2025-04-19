#pragma once
#include <raygui-cpp.h>

class Game;

class GUI
{
public:
	GUI(Game& game);
	void drawGame(Game& game, int cellSize, int rows, float energy);
	void drawMenu(Game& game);
private:
	void initMenu(Game& game);
	void initGame(Game& game);

	void drawEnergyBar(int cellSize, int rows, float energy);

	rgc::Button m_playButton;
	rgc::Button m_exitButton;

	rgc::Button m_menuButton;
	rgc::ProgressBar m_energyBar;
};