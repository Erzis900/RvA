#pragma once
#include "IGameState.h"
#include <raygui.h>

class Game;

class LostState : public IGameState
{
public:
	LostState();
	void draw(Game& game) override;

	void onEnter(Game& game) override;
	void onExit(Game& game) override;
private:
	Vector2 btnSize;
};