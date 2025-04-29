#include "WinState.h"
#include "MenuState.h"
#include "Game.h"
#include <raylib.h>

void WinState::draw(Game& game)
{
	auto& guiHelper = game.getGUIHelper();
	guiHelper.DrawText({ "You Won!!!", 30, WHITE, { {0, 0}, GUIAlignmentH::Center, GUIAlignmentV::Center } });

	guiHelper.DrawText({ "Press any key to continue", 18, WHITE, { {0, 10}, GUIAlignmentH::Center, GUIAlignmentV::Bottom } });
}

void WinState::update(Game& game, float dt)
{
	if(
		GetKeyPressed() ||
		IsMouseButtonDown(MOUSE_LEFT_BUTTON) ||
		IsMouseButtonDown(MOUSE_RIGHT_BUTTON) ||
		IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
		game.setState(std::make_unique<MenuState>());
	}
}