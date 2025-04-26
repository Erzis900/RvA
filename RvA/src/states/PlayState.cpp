#include "PlayState.h"
#include <raylib.h>
#include "Game.h"
#include "constants.h"

PlayState::PlayState(Game& game)
	:m_cellSize(32), m_cols(COLS), m_rows(ROWS), m_energy(100.f), m_enemyManager(game), m_defenderManager(game)
{
}

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

void PlayState::update(Game& game, float dt)
{
	if (game.getGUI().isPaused()) return;

	m_enemyManager.update(dt, m_cellSize, m_rows);
	m_defenderManager.update(dt, m_cellSize, m_rows, m_energy, game.getGUI().getBatteries(), m_enemyManager);
}

void PlayState::draw(Game& game)
{
	drawGrid();

	m_defenderManager.draw(m_cellSize);
	m_enemyManager.draw(m_cellSize);

	game.getGUI().drawGame(m_cellSize, m_rows, m_energy, m_defenderManager);
}