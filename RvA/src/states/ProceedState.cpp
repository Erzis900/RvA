#include "ProceedState.h"

ProceedState::ProceedState(std::function<void()> onEnter) : m_onEnter(std::move(onEnter)) {}

flow::FsmAction ProceedState::enter() {
	m_onEnter();
	return flow::FsmAction::transition("proceed");
}

void ProceedState::exit() {}

flow::FsmAction ProceedState::update(float dt) {
	return flow::FsmAction();
}
