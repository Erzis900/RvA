#pragma once

#include "IGameState.h"
#include <raylib.h>
#include <vector>
#include <string>

struct CreditsItem {
	std::string label;
	unsigned int fontSize;
	Color color;
};

class CreditsState : public IGameState
{
public:
	void draw(Game& game) override;

private:
	static const std::vector<CreditsItem> creditsItems;
};