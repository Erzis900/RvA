#pragma once

#include <functional>

class DelayedAction {
public:
	void start(float delay, std::function<void()> action);
	void update(float dt);

private:
	float m_delay;
	float m_time{0.f};
	bool m_isStarted{false};
	std::function<void()> m_action;
};
