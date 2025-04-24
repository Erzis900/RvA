#pragma once
#include "IGameState.h"
#include <raygui.h>

class Game;

class LostState : public IGameState
{
public:
	LostState(Game& game);
	void draw(Game& game) override;
private:
	Vector2 btnSize;
};