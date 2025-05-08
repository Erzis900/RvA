#pragma once

#include "fsm/FsmState.h"

#include <raygui.h>

class Game;

class MenuState : public flow::FsmState {
public:
	MenuState(Game& game);

	flow::FsmAction update(float dt) override;
	flow::FsmAction enter() override;
	void exit() override;

private:
	Game& m_game;
	std::string m_nextTransition;
};
