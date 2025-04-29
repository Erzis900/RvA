#pragma once
#include "IGameState.h"
#include <raygui.h>

class Game;

class MenuState : public IGameState
{
public:
	void draw(Game& game) override;
};
