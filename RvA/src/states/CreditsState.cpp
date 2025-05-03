#include "CreditsState.h"

#include <raylib.h>
#include <vector>
#include <string>

#include "Game.h"
#include "states/MenuState.h"

struct CreditsItem {
	std::string label;
	unsigned int fontSize;
	Color color;
};

// TODO(Gerark) - This list could be defined in data ( maybe a json file to easily edit it without recompiling )
const std::vector<CreditsItem> creditsItems {
	{ "CREDITS", 20, WHITE },
	{},
	{ "PROGRAMMING", 18, WHITE },
	{ "Erzis", 18, DARKGRAY },
	{ "Zazeraz", 18, DARKGRAY },
	{ "Gerark", 18, DARKGRAY },
	{},
	{ "PIXEL ART", 18, WHITE },
	{ "Marvin", 18, DARKGRAY },
	{},
	{ "CONCEPT ART", 18, WHITE },
	{ "yeahno", 18, DARKGRAY },
	{},
	{ "MUSIC", 18, WHITE },
	{ "Trim", 18, DARKGRAY },
	{ "POG LIZARD", 18, DARKGRAY },
};

void CreditsState::onEnter(Game& game)
{
	auto builder = game.getGUI().buildScreen("Credits");
	builder.stack({ .orientation = GUIOrientation::Vertical, .horizontalAlignment = GUIAlignmentH::Center, .verticalAlignment = GUIAlignmentV::Center, .size = { 100.f, autoSize } });

	for (auto& item : creditsItems)
	{
		if (!item.label.empty())
		{
			builder.text({
				.text = item.label.c_str(),
				.fontSize = item.fontSize,
				.color = item.color,
				.horizontalAlignment = GUIAlignmentH::Center
			});
		}
		else
		{
            builder.space({ 0, 10.f });
		}
	}

    builder.button({ "Back", {}, {autoSize, 40.f}, [&game]() { game.setState<MenuState>(); } });
	builder.end();
}

void CreditsState::onExit(Game& game)
{
    game.getGUI().destroyScreen("Credits");
}
