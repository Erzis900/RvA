#include "MenuState.h"
#include "Game.h"
#include "states/PlayState.h"
#include "states/CreditsState.h"

void MenuState::onEnter(Game& game)
{
	game.getMusicManager().play(game.getMusicManager().getMenuMusic());
}

void MenuState::onExit(Game& game)
{
	game.getMusicManager().stop(game.getMusicManager().getMenuMusic());
}

void MenuState::draw(Game& game)
{
	auto btnSize = Vector2{ 200.f, 60.f };
	auto& gui = game.getGUI();
	if (gui.drawButton({ "Exit", btnSize, { {0, btnSize.y}, GUIAlignmentH::Center, GUIAlignmentV::Center } }))
	{
		exit(0);
	}

	if (gui.drawButton({ "Credits", btnSize, { {0, 0}, GUIAlignmentH::Center, GUIAlignmentV::Center } }))
	{
		game.setState(std::make_unique<CreditsState>());
	}

	if (gui.drawButton({ "Play", btnSize, { {0, -btnSize.y}, GUIAlignmentH::Center, GUIAlignmentV::Center } }))
	{
		game.setState(std::make_unique<PlayState>(game));
	}
}
