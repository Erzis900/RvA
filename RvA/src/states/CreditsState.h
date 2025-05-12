#pragma once

#include "fsm/FsmState.h"

class GUI;

class CreditsState : public flow::FsmState {
public:
	CreditsState(GUI& gui);

	flow::FsmAction update(float dt) override;
	flow::FsmAction enter() override;
	void exit() override;

private:
	GUI& m_gui;
	std::string m_nextTransition;
};
