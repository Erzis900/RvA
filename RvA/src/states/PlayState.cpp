#include "PlayState.h"
#include "WinState.h"
#include "LostState.h"
#include <raylib.h>
#include <raymath.h>
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
	: m_game(game), m_defenderManager(game.getAtlas(), game.getGUI()), m_enemyManager(game, m_defenderManager), m_bulletManager(m_enemyManager)
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

	performDefenderSpawnOnInput();
	auto result = m_defenderManager.update(dt);
	performActions(result.actions);
	updateEnergyAndBatteries(result.amountOfBatteryGain, result.amountOfEnergyDrain);
	if (m_energy <= 0)
	{
		m_game.setState(std::make_unique<LostState>());
	}

	m_bulletManager.update(dt);

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
	m_bulletManager.draw();

	game.getGUI().drawGame(m_energy, m_batteries);
}

void PlayState::goToWinState(Game& game)
{
	game.setState(std::make_unique<WinState>(game));
}

void PlayState::updateEnergyAndBatteries(float batteryGain, float energyDrain)
{
	m_batteries += batteryGain;
	m_energy -= energyDrain;
	m_energy = Clamp(m_energy, 0, 100);
}

void PlayState::performDefenderSpawnOnInput()
{
	if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)))
	{
		auto mousePos = GetMousePosition();
		int row = int(mousePos.y) / CELL_SIZE - 1;
		int column = int(mousePos.x) / CELL_SIZE - 1;

		if (row >= 0 && row < ROWS && column >= 0 && column < COLS)
		{
			auto type = m_game.getGUI().getSelectedDefender();
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && type != DefenderType::None)
			{
				if (canPlaceDefender(row, column))
				{
					auto defenderInfo = m_game.getDefenderRegistry().getDefenderInfo(type);
					if (defenderInfo && canAffordCost(defenderInfo->cost))
					{
						m_defenderManager.spawnDefender(defenderInfo, row, column);
						m_batteries -= defenderInfo->cost;
					}
				}
			}
			else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
			{
				if (!canPlaceDefender(row, column))
				{
					m_defenderManager.toggleDefender(row, column);
				}
			}
		}
	}
}

void PlayState::performActions(const Actions& actions)
{
	for (auto& action : actions)
	{
		std::visit([this](auto&& action) { performAction(action); }, action);
	}
}

void PlayState::performAction(const BulletSpawnAction& action)
{
	auto bulletInfo = m_game.getBulletTypeRegistry().getBulletInfo(action.bulletType);
	if (bulletInfo)
	{
		m_bulletManager.spawnBullet(*bulletInfo, action.position);
	}
}

bool PlayState::canAffordCost(int cost) const
{
	return cost <= m_batteries;
}

bool PlayState::canPlaceDefender(int x, int y) const
{
	return !m_defenderManager.hasDefender(x, y);
}