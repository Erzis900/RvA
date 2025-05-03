#include "MenuState.h"
#include "Game.h"
#include "states/PlayState.h"
#include "states/CreditsState.h"
#include "states/OptionsState.h"

void MenuState::onEnter(Game& game)
{
	game.getMusicManager().play(game.getMusicManager().getMenuMusic());

	auto btnSize = Vector2{ autoSize, 40.f };
	auto& gui = game.getGUI();
	gui.buildScreen("MainMenu")
		.stack({ .orientation = GUIOrientation::Vertical, .horizontalAlignment = GUIAlignmentH::Center, .verticalAlignment = GUIAlignmentV::Center, .size = { 100.f, autoSize } })
		    .text({ .text = "RvA", .fontSize = 20, .color = WHITE, .horizontalAlignment = GUIAlignmentH::Center })
			.space({ 0, 40.f })
			.button({ "Play", {}, btnSize, [&game]() { game.setState<PlayState>(game); } })
			.button({ "Options", {}, btnSize, [&game]() { game.setState<OptionsState>(); } })
			.button({ "Credits", {}, btnSize, [&game]() { game.setState<CreditsState>(); } })
			.button({ "Exit", {}, btnSize, [&game]() {  game.scheduleClose(); } })
		.end();
}

void MenuState::onExit(Game& game)
{
	game.getMusicManager().stop(game.getMusicManager().getMenuMusic());
    game.getGUI().destroyScreen("MainMenu");
}

void MenuState::draw(Game& game)
{
}
