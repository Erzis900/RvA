#include "LostState.h"
#include "Game.h"
#include "states/MenuState.h"
#include "constants.h"

LostState::LostState()
	:btnSize({ 200.f, 60.f })
{}

void LostState::onEnter(Game& game)
{
	game.getMusicManager().play(game.getMusicManager().getLostMusic());
}

void LostState::onExit(Game& game)
{
	game.getMusicManager().stop(game.getMusicManager().getLostMusic());
}

void LostState::draw(Game& game)
{
	DrawText("YOU LOST", int(TEX_WIDTH / 2 - MeasureText("YOU LOST!", 20) / 2), int(TEX_HEIGHT / 2 - 100), 20, WHITE);
	if (GuiButton({ TEX_WIDTH / 2 - btnSize.x / 2, TEX_HEIGHT / 2, btnSize.x, btnSize.y }, "Menu"))
	{
		game.setState(std::make_unique<MenuState>());
	}
}