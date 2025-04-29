#pragma once

#include "IGameState.h"

class WinState : public IGameState
{
public:
	void draw(Game& game) override;
	void update(Game& game, float dt) override;
};
