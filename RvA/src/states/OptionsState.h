#pragma once

#include "IGameState.h"
#include <raylib.h>

class OptionsState : public IGameState
{
public:
	void draw(Game& game) override;
	void onEnter(Game& game) override;
	void onExit(Game& game) override;
};
