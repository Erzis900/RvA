#include "OptionsState.h"
#include "Game.h"
#include "states/MenuState.h"


void OptionsState::onEnter(Game& game)
{
	game.getMusicManager().play(game.getMusicManager().getMenuMusic());
}

void OptionsState::onExit(Game& game)
{
	game.getMusicManager().stop(game.getMusicManager().getMenuMusic());
}

void OptionsState::draw(Game& game)
{
	auto& gui = game.getGUI();

	if (gui.drawButton({
		.text = "Toggle Music",
		.size = {200.f, 60.f},
		.guiPosition = {
			.position = { 10, 10 },
			.horizontalAlignment = GUIAlignmentH::Center,
			.verticalAlignment = GUIAlignmentV::Center,
		} }))
	{
		if (GetMasterVolume() > 0.f)
		{
			SetMasterVolume(0.f);
		} 
		else
		{
			SetMasterVolume(1.0f);
		}
	}

	if (gui.drawButton({
		.text = "Back to Main Menu",
		.size = {200.f, 60.f},
		.guiPosition = {
			.position = { 10, 70 },
			.horizontalAlignment = GUIAlignmentH::Center,
			.verticalAlignment = GUIAlignmentV::Center,
		} }))
	{
		game.setState(std::make_unique<MenuState>());
	}
}
