#pragma once
#include "IGameState.h"
#include <raygui.h>

class Game;

class LostState : public IGameState
{
public:
	void onEnter(Game& game) override;
	void onExit(Game& game) override;
private:
	void restart(Game& game);

	Vector2 btnSize;
};