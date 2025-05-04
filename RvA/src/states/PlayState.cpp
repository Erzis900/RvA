#include "PlayState.h"
#include "WinState.h"
#include "LostState.h"
#include "MenuState.h"
#include "PauseState.h"
#include "Game.h"
#include "constants.h"

PlayState::PlayState(Game& game)
	: m_game(game)
	, m_session(m_game.getGameSession())
{
}

PlayState::~PlayState()
{
}

void PlayState::onEnter(Game& game)
{
	m_session.start();
	game.getMusicManager().play(game.getMusicManager().getGameMusic());
}

void PlayState::onExit(Game& game)
{
	game.getMusicManager().stop(game.getMusicManager().getGameMusic());
}

void PlayState::update(Game& game, float dt)
{
	if (!m_isGamePaused)
	{
		m_session.update(game, dt);

		if (m_session.getNumberOfDestroyedEnemies() >= m_numberOfEnemiesToKill) {
			goToWinState();
		}

		if (m_session.getBatteryCharge() <= 0)
		{
			m_game.setState<LostState>();
		}

		if constexpr (DEV_MODE)
		{
			if (IsKeyPressed(KEY_W))
			{
				goToWinState();
			}
			else if (IsKeyPressed(KEY_F1))
			{
				m_session.getCollisionSystem().toggleDebugView();
			}
		}
	}

	if (IsKeyPressed(KEY_ESCAPE))
	{
		goToPauseState();
	}
}

void PlayState::draw(Game& game)
{
	m_session.draw(game.getAtlas());
}

void PlayState::goToWinState()
{
	m_game.setState<WinState>(m_game);
}

void PlayState::goToPauseState()
{
	m_game.setState<PauseState, false>(m_session);
}
