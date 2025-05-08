#include "DelayedAction.h"

void DelayedAction::start(float delay, std::function<void()> action) {
	m_action = std::move(action);
	m_delay = delay;
	m_time = 0.f;
	m_isStarted = true;
}

void DelayedAction::update(float dt) {
	if (m_isStarted) {
		m_time += dt;
		if (m_time >= m_delay) {
			m_action();
			m_time = 0.f;
			m_isStarted = false;
		}
	}
}
