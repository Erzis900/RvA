#pragma once

#include "fsm/FsmState.h"

class Game;

class EndScreenState : public flow::FsmState {
public:
	EndScreenState(Game& game);

	flow::FsmAction update(float dt) override;
	flow::FsmAction enter() override;
	void exit() override;

private:
	Game& m_game;
	std::string m_nextTransition;
};
