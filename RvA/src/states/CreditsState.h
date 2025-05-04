#pragma once

#include "fsm/FsmState.h"
#include "IGameState.h"

class CreditsState : public IGameState, public flow::FsmState
{
public:
    CreditsState(Game& game);

	flow::FsmAction update(float dt) override;
	flow::FsmAction enter() override;
	void exit() override;

private:
	Game& m_game;
    std::string m_nextTransition;
};
