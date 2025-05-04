#include "states/PauseState.h"

#include "Game.h"
#include "states/PlayState.h"
#include "states/MenuState.h"
#include "states/OptionsState.h"

PauseState::PauseState(Session& gameSession) : m_gameSession(gameSession)
{
}

void PauseState::onEnter(Game& game)
{
	game.getGameSession().setPause(true);

	auto btnSize = Vector2{ autoSize, 40.f };
	auto& gui = game.getGUI();
	gui.buildScreen("Pause")
		.rect({ 0, 0, TEX_WIDTH, TEX_HEIGHT }, Fade(BLACK, 0.5f))
		.vertical_stack(5, 200.f)
			.text({ .text = "Options", .fontSize = 20, .color = WHITE, .horizontalAlignment = GUIAlignmentH::Center })
			.space({ 0, 40.f })
			.button({ "Resume", {}, btnSize, [&game]() { game.setState<PlayState, false>(game); } })
			.button({ "Restart", {}, btnSize, [this, &game]() { restart(game); }})
			.button({ "Exit to Menu", {}, btnSize, [this, &game]() { exitGameSession(game); } })
		.end();
}

void PauseState::onExit(Game& game)
{
    game.getGUI().destroyScreen("Pause");
}

void PauseState::exitGameSession(Game& game)
{
    m_gameSession.end();
	game.setState<MenuState>();
}

void PauseState::restart(Game& game)
{
    m_gameSession.end();
    game.setState<PlayState, false>(game);
}

void PauseState::update(Game& game, float dt)
{
    if (IsKeyPressed(KEY_ESCAPE)) {
        game.setState<PlayState, false>(game);
    }
}

void PauseState::draw(Game& game)
{
    m_gameSession.draw(game.getAtlas());
}