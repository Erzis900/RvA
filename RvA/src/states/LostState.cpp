#include "LostState.h"
#include "Game.h"
#include "states/MenuState.h"
#include "constants.h"

LostState::LostState()
	:btnSize({ 200.f, 60.f })
{}

void LostState::draw(Game& game)
{
	DrawText("YOU LOST", int(TEX_WIDTH - MeasureText("YOU LOST!", 20) / 2), int(TEX_HEIGHT - 100), 20, WHITE);
	if (GuiButton({ TEX_WIDTH - btnSize.x / 2, TEX_HEIGHT, btnSize.x, btnSize.y }, "Menu"))
	{
		game.setState(std::make_unique<MenuState>());
	}
}