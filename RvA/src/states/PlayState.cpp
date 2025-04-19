#include "PlayState.h"
#include <raylib.h>
#include "Game.h"

PlayState::PlayState(Game& game)
	:m_cellSize(32), m_cols(18), m_rows(10), m_energy(100.f)
{ }

void PlayState::drawGrid()
{
	for (int row = 0; row < m_rows; row++) {
		for (int col = 0; col < m_cols; col++) {
			int x = m_cellSize + col * m_cellSize;
			int y = m_cellSize + row * m_cellSize;

			DrawRectangleLines(x, y, m_cellSize, m_cellSize, BLACK);
		}
	}
}

void PlayState::update(Game& game)
{
}

void PlayState::draw(Game& game, GUI& gui)
{
	drawGrid();
	gui.drawGame(game, m_cellSize, m_rows, m_energy);
}