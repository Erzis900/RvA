#pragma once

#include "fsm/FsmState.h"
#include "utilities/CallbackRegistry.h"

class Session;
class HUD;
class Game;

class PlayState : public flow::FsmState {
public:
	PlayState(Game& game);

	flow::FsmAction update(float dt) override;
	flow::FsmAction enter() override;
	void exit() override;

private:
	int m_numberOfEnemiesToKill{2000};
	Game& m_game;
};
