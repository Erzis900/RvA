#include "CreditsState.h"
#include "Game.h"
#include "states/MenuState.h"

// TODO(Gerark) - This list could be defined in data ( maybe a json file to easily edit it without recompiling )
const std::vector<CreditsItem> CreditsState::creditsItems{
	{ "CREDITS", 24, WHITE },
	{},
	{ "PROGRAMMING", 20, WHITE },
	{ "Erzis", 20, DARKGRAY },
	{ "Zazeraz", 20, DARKGRAY },
	{ "Gerark", 20, DARKGRAY },
	{},
	{ "PIXEL ART", 20, WHITE },
	{ "Marvin", 20, DARKGRAY },
	{},
	{ "CONCEPT ART", 20, WHITE },
	{ "yeahno", 20, DARKGRAY },
	{},
	{ "MUSIC", 20, WHITE },
	{ "Trim", 20, DARKGRAY },
	{ "POG LIZARD", 20, DARKGRAY },
};

void CreditsState::draw(Game& game)
{
	auto& gui = game.getGUI();

	auto y = 25.f;
	for (auto& item : creditsItems)
	{
		if (!item.label.empty())
		{
			gui.drawText({
				.text = item.label.c_str(),
				.fontSize = item.fontSize,
				.color = item.color,
				.guiPosition = {
					.position = {0, y},
					.horizontalAlignment = GUIAlignmentH::Center
				}
			});
			y += item.fontSize;
		}
		else
		{
			y += 15;
		}
	}

	if (gui.drawButton({
		.text = "Back",
		.size = {100.f, 40.f},
		.guiPosition = {
			.position = { 10, 10 },
			.horizontalAlignment = GUIAlignmentH::Right,
			.verticalAlignment = GUIAlignmentV::Bottom,
		}}))
	{
		game.setState(std::make_unique<MenuState>());
	}
}
