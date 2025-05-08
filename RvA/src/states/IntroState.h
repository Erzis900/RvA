#pragma once

#include "IGameState.h"
#include "fsm/FsmState.h"

#include <raygui.h>

class Game;

class IntroState : public IGameState, public flow::FsmState {
public:
	IntroState(Game& game);

	flow::FsmAction update(float dt) override;
	flow::FsmAction enter() override;
	void exit() override;

private:
	std::string m_nextTransition;
	Game& m_game;

	float m_displayTime{2.f};
};
