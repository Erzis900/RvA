#include "PlayState.h"
#include <raylib.h>
#include "Game.h"
#include "constants.h"

PlayState::PlayState(Game& game)
	:m_cols(COLS), m_rows(ROWS), m_energy(MAX_ENERGY), m_enemyManager(game), m_defenderManager(game)
{
}

void PlayState::drawGrid()
{
	for (int row = 0; row < m_rows; row++) {
		for (int col = 0; col < m_cols; col++) {
			int x = CELL_SIZE + col * CELL_SIZE;
			int y = CELL_SIZE + row * CELL_SIZE;

			DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, BLACK);
		}
	}
}

void PlayState::update(Game& game, float dt)
{
	if (game.getGUI().isPaused()) return;

	m_enemyManager.update(dt);
	m_defenderManager.update(dt, m_energy, game.getGUI().getBatteries(), m_enemyManager);
}

void PlayState::draw(Game& game)
{
	drawGrid();

	m_defenderManager.draw();
	m_enemyManager.draw();

	game.getGUI().drawGame(m_energy, m_defenderManager);
}