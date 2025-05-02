#include "TimedAction.h"

TimedAction::TimedAction(std::function<void()> action, float duration)
	: m_time(duration)
	, m_action(std::move(action))
{
}

void TimedAction::updateTimer(float dt)
{
	m_time -= dt;
}

void TimedAction::callAction()
{
	m_action();
}

bool TimedAction::isOver() const
{
	return m_time <= 0;
}

void TimedActionList::addTimedAction(std::function<void()> action, float duration)
{
	m_actions.push_back(std::make_unique<TimedAction>(std::move(action), duration));
}

void TimedActionList::updateTimers(float dt)
{
	for (auto it = m_actions.begin(); it != m_actions.end();)
	{
		auto& action = *it;
		action->updateTimer(dt);
		if (action->isOver())
		{
			it = m_actions.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void TimedActionList::callActions()
{
	for (auto& action : m_actions)
	{
		action->callAction();
	}
}
