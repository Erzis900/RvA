#pragma once

#include "fsm/FSMState.h"

#include <functional>

class ProceedState : public flow::FsmState {
public:
	ProceedState(std::function<void()> onEnter);

	flow::FsmAction enter() override;
	void exit() override;
	flow::FsmAction update(float dt) override;

private:
	std::function<void()> m_onEnter;
};
