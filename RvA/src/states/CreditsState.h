#pragma once

#include "IGameState.h"
#include <raylib.h>
#include <vector>
#include <string>

struct CreditsItem {
	std::string label;
	int fontSize;
	Color color;
};

class CreditsState : public IGameState
{
public:
	void draw(Game& game) override;

private:
	// TODO(Gerark) - These methods could be moved to a more generic place ( Maybe a little UI helper? )
	void drawCenteredText(const char* text, Color color, int fontSize, int y, float renderTextureWidth);
	int getCenteredTextPosition(const char* text, int fontSize, float renderTextureWidth);

	static const std::vector<CreditsItem> creditsItems;
};