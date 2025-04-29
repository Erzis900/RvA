#include "PlayState.h"
#include "WinState.h"
#include <raylib.h>
#include "Game.h"
#include "constants.h"

void PlayState::onEnter(Game& game)
{
	game.getMusicManager().play(game.getMusicManager().getGameMusic());
}

void PlayState::onExit(Game& game)
{
	game.getMusicManager().stop(game.getMusicManager().getGameMusic());
}

PlayState::PlayState(Game& game)
	: m_enemyManager(game), m_defenderManager(game)
{
	m_enemyManager.onEnemiesDestroyed([this, &game](int numberOfDestroyedEnemies) {
		m_numberOfDestroyedEnemies += numberOfDestroyedEnemies;
		if (m_numberOfDestroyedEnemies >= m_numberOfEnemiesToKill) {
			goToWinState(game);
		}
	});
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

	if constexpr (DEV_MODE)
	{
		if (IsKeyPressed(KEY_W))
		{
			goToWinState(game);
		}
	}
}

void PlayState::draw(Game& game)
{
	drawGrid();

	m_defenderManager.draw();
	m_enemyManager.draw();

	game.getGUI().drawGame(m_energy, m_defenderManager);
}

void PlayState::goToWinState(Game& game)
{
	game.setState(std::make_unique<WinState>());
}
