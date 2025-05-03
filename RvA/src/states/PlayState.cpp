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
	if (!m_isGamePaused)
	{
		m_session.update(game, dt);

		if (m_session.getNumberOfDestroyedEnemies() >= m_numberOfEnemiesToKill) {
			goToWinState(game);
		}

		if (m_session.getBatteryCharge() <= 0)
		{
			m_game.setState<LostState>();
		}

		updateHud();

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

	if (IsKeyPressed(KEY_ESCAPE))
	{
		togglePause();
	}
}

void PlayState::draw(Game& game)
{
	m_session.draw(game);
	m_hud.draw();
}

void PlayState::goToWinState(Game& game)
{
	game.setState<WinState>(game);
}

void PlayState::setupHUD()
{
	auto& hudData = m_hud.data();
	for (const auto& [type, defenderInfo] : m_game.getDefenderRegistry().getDefenderInfos())
	{
		hudData.defenders.emplace_back(type, defenderInfo.spriteEnabled.spriteInfo, defenderInfo.cost);
	}

	m_hud.onMenuButtonPressed([this]() { m_game.setState<MenuState>(); });
	m_hud.onPauseButtonPressed([this]() { togglePause(); });
	m_hud.onResumeButtonPressed([this]() { togglePause(); });
	m_hud.onDefenderSelected([this]() { m_session.setSelectedDefender(m_hud.data().selectedDefender); });
}

void PlayState::updateHud()
{
	auto& hudData = m_hud.data();
	hudData.batteryCharge = m_session.getBatteryCharge();
	hudData.scrapsAmount = static_cast<int>(m_session.getScraps());

	// TODO(Gerark) - Not very optimal but we'll see if it's going to be ever a bottleneck.
	hudData.progressBars.clear();
	for (auto& defender : m_session.getDefenderManager().getDefenders())
	{
		hudData.progressBars.push_back(ProgressBarData{
			.value = static_cast<float>(defender->hp),
			.max = static_cast<float>(defender->info->maxHP),
			.position = defender->position,
			.bkgColor = DARKGRAY,
			.fillColor = GREEN
		});
	}

	for (auto& enemy : m_session.getEnemyManager().getEnemies())
	{
		hudData.progressBars.push_back(ProgressBarData{
			.value = enemy->getHp(),
			.max = enemy->getInfo()->maxHp,
			.position = enemy->getPosition(),
			.bkgColor = DARKGRAY,
			.fillColor = GREEN
		});
	}
}

void PlayState::togglePause()
{
	m_isGamePaused = !m_isGamePaused;
	m_hud.data().drawPause = m_isGamePaused;
}
