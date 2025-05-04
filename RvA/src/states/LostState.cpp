#include "LostState.h"

#include "Game.h"
#include "states/MenuState.h"
#include "states/PlayState.h"
#include "constants.h"

void LostState::onEnter(Game& game)
{
	game.getGameSession().end();
	game.getMusicManager().play(game.getMusicManager().getLostMusic());

	auto btnSize = Vector2{ autoSize, 40.f };
	auto& gui = game.getGUI();
	gui.buildScreen("Lose")
		.vertical_stack(5, 200.f)
		    .text({ .text = "You Lost!", .fontSize = 20, .color = WHITE, .horizontalAlignment = GUIAlignmentH::Center })
			.space({ 0, 35.f })
			.button({ "Restart", {}, btnSize, [this, &game]() { game.getMusicManager().play(game.getMusicManager().getButtonClick()); restart(game); }})
			.button({ "Menu", {}, btnSize, [&game]() { game.getMusicManager().play(game.getMusicManager().getButtonClick()); game.setState<MenuState>(); } })
		.end();
}

void LostState::onExit(Game& game)
{
	game.getMusicManager().stop(game.getMusicManager().getLostMusic());
    game.getGUI().destroyScreen("Lose");
}

void LostState::restart(Game& game)
{
    game.getGameSession().end();
    game.setState<PlayState>(game);
}
