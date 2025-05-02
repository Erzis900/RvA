#pragma once

#include <functional>
#include <memory>

class TimedAction
{
public:
	TimedAction(std::function<void()> action, float duration);

	void updateTimer(float dt);
	void callAction();
	bool isOver() const;

private:
	float m_time;
	std::function<void()> m_action;
};

class TimedActionList
{
public:
	void addTimedAction(std::function<void()> action, float duration);
	void updateTimers(float dt);
	void callActions();

private:
	std::vector<std::unique_ptr<TimedAction>> m_actions;
};