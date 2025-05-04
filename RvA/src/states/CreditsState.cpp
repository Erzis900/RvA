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
	{ "Credits", 20, WHITE },
	{},
	{ "PROGRAMMING", 8, WHITE },
	{ "Erzis", 8, DARKGRAY },
	{ "Zazeraz", 8, DARKGRAY },
	{ "Gerark", 8, DARKGRAY },
	{},
	{ "PIXEL ART", 8, WHITE },
	{ "Marvin", 8, DARKGRAY },
	{},
	{ "CONCEPT ART", 8, WHITE },
	{ "yeahno", 8, DARKGRAY },
	{},
	{ "MUSIC", 8, WHITE },
	{ "Trim", 8, DARKGRAY },
	{ "POG LIZARD", 8, DARKGRAY },
};

void CreditsState::onEnter(Game& game)
{
	auto builder = game.getGUI().buildScreen("Credits");
	builder.vertical_stack(2, 100.f);

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

    builder.space({ 0, 20.f });
    builder.button({ "Back", {}, {autoSize, 40.f}, [&game]() { game.setState<MenuState>(); } });
	builder.end();
}

void CreditsState::onExit(Game& game)
{
    game.getGUI().destroyScreen("Credits");
}
