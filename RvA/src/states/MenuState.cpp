#include "MenuState.h"
#include "Game.h"
#include "states/PlayState.h"

MenuState::MenuState(Game& game)
	:btnSize({ 200.f, 60.f })
{}

void MenuState::draw(Game& game)
{
	if (GuiButton({ game.getTexSize().x / 2 - btnSize.x / 2, game.getTexSize().y / 2, btnSize.x, btnSize.y }, "Exit"))
	{
		exit(0);
	}

	if (GuiButton({ game.getTexSize().x / 2 - btnSize.x / 2, game.getTexSize().y / 2 - btnSize.y, btnSize.x, btnSize.y }, "Play"))
	{
		game.getGUI().reset();
		game.setState(std::make_unique<PlayState>(game));
	}
}