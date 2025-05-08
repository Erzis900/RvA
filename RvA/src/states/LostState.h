#pragma once

#include "fsm/FsmState.h"

class Game;

class LostState : public flow::FsmState {
public:
	LostState(Game& game);

	flow::FsmAction update(float dt) override;
	flow::FsmAction enter() override;
	void exit() override;

private:
	void restart();

	std::string m_nextTransition;
	Game& m_game;
};
