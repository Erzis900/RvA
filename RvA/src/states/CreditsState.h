#pragma once

#include "IGameState.h"

class CreditsState : public IGameState
{
public:
	void onEnter(Game& game) override;
	void onExit(Game& game) override;
};
