#include "PlayState.h"
#include "WinState.h"
#include "LostState.h"
#include "MenuState.h"
#include "Game.h"
#include "constants.h"

PlayState::PlayState(Game& game)
	: m_game(game)
	, m_hud(m_game.getAtlas(), m_game.getGUI())
	, m_session(m_game, m_collisionSystem)
{
	setupHUD();
}

PlayState::~PlayState()
{
}

void PlayState::onEnter(Game& game)
{
	game.getMusicManager().play(game.getMusicManager().getGameMusic());
}

void PlayState::onExit(Game& game)
{
	game.getMusicManager().stop(game.getMusicManager().getGameMusic());
}

void PlayState::update(Game& game, float dt)
{
	if (m_isGamePaused) return;

	m_session.update(game, dt);

	if (m_session.getNumberOfDestroyedEnemies() >= m_numberOfEnemiesToKill) {
		goToWinState(game);
	}

	if (m_session.getBatteryCharge() <= 0)
	{
		m_game.setState(std::make_unique<LostState>());
	}

	m_hud.data().batteryCharge = m_session.getBatteryCharge();
	m_hud.data().scrapsAmount = static_cast<int>(m_session.getScraps());

	if constexpr (DEV_MODE)
	{
		if (IsKeyPressed(KEY_W))
		{
			goToWinState(game);
		}
		else if (IsKeyPressed(KEY_F1))
		{
			m_collisionSystem.toggleDebugView();
		}
	}
}

void PlayState::draw(Game& game)
{
	m_session.draw(game);
	m_hud.draw();
}

void PlayState::goToWinState(Game& game)
{
	game.setState(std::make_unique<WinState>(game));
}

void PlayState::setupHUD()
{
	auto& hudData = m_hud.data();
	for (const auto& [type, defenderInfo] : m_game.getDefenderRegistry().getDefenderInfos())
	{
		hudData.defenders.emplace_back(type, defenderInfo.spriteEnabled.spriteInfo, defenderInfo.cost);
	}

	m_hud.onMenuButtonPressed([this]() { m_game.setState(std::make_unique<MenuState>()); });
	m_hud.onPauseButtonPressed([this]() { togglePause(); });
	m_hud.onResumeButtonPressed([this]() { togglePause(); });
	m_hud.onDefenderSelected([this]() { m_session.setSelectedDefender(m_hud.data().selectedDefender); });
}

void PlayState::togglePause()
{
	m_isGamePaused = !m_isGamePaused;
	m_hud.data().drawPause = m_isGamePaused;
}
