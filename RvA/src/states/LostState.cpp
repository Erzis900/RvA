#include "LostState.h"
#include "Game.h"
#include "states/MenuState.h"

LostState::LostState(Game& game)
	:btnSize({ 200.f, 60.f })
{
}

void LostState::draw(Game& game)
{
	DrawText("YOU LOST", game.getTexSize().x / 2 - float(MeasureText("YOU LOST!", 20)) / 2, game.getTexSize().y / 2 - 100, 20, WHITE);
	if (GuiButton({ game.getTexSize().x / 2 - btnSize.x / 2, game.getTexSize().y / 2, btnSize.x, btnSize.y }, "Menu"))
	{
		game.setState(std::make_unique<MenuState>(game));
	}
}