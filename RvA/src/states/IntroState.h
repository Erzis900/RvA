#pragma once

#include "fsm/FsmState.h"
#include "utilities/DelayedAction.h"

#include <raygui.h>

class Game;

class IntroState : public flow::FsmState {
public:
	IntroState(Game& game);

	flow::FsmAction update(float dt) override;
	flow::FsmAction enter() override;
	void exit() override;

private:
	std::string m_nextTransition;
	Game& m_game;

	DelayedAction m_exitTime;
};
