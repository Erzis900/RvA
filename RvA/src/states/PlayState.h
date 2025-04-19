#pragma once
#include "IGameState.h"
#include "raygui-cpp.h"
#include "GUI/GUI.h"

class Game;

class PlayState : public IGameState
{
public:
	PlayState(Game& game);

	void update(Game& game) override;
	void draw(Game& game, GUI& gui) override;
private:
	void drawGrid();

	int m_rows;
	int m_cols;
	int m_cellSize;

	float m_energy;
};