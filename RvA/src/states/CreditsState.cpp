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
	auto y = 25;
	for (auto& item : creditsItems)
	{
		if (!item.label.empty())
		{
			drawCenteredText(item.label.c_str(), item.color, item.fontSize, y, game.getTexSize().x);
			y += item.fontSize;
		}
		else
		{
			y += 15;
		}
	}

	const Vector2 buttonSize = { 100.f, 40.f };
	if (GuiButton({ game.getTexSize().x - (buttonSize.x + 10), game.getTexSize().y - (buttonSize.y + 10), buttonSize.x, buttonSize.y}, "Back"))
	{
		game.setState(std::make_unique<MenuState>(game));
	}
}

void CreditsState::drawCenteredText(const char* text, Color color, int fontSize, int y, float renderTextureWidth)
{
	DrawText(text, getCenteredTextPosition(text, fontSize, renderTextureWidth), y, fontSize, color);
}

int CreditsState::getCenteredTextPosition(const char* text, int fontSize, float renderTextureWidth)
{
	return static_cast<int>(renderTextureWidth / 2 - MeasureText(text, fontSize) / 2);
}