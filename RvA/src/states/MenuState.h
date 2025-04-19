#pragma once
#include "IGameState.h"
#include <raygui-cpp.h>
#include <iostream>

class Game;

class MenuState : public IGameState
{
public:
	MenuState(Game& game);
	void draw(Game& game, GUI& gui) override;
};