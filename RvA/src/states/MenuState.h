#pragma once
#include "IGameState.h"
#include <raygui.h>

class Game;

class MenuState : public IGameState
{
public:
	MenuState();
	void draw(Game& game) override;
private:
	Vector2 btnSize;
};