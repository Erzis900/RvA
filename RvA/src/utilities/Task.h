#pragma once

#include <functional>

class Task {
public:
	Task(std::function<bool(float)> task) : m_task(std::move(task)) {}

	void execute(float dt) {
		if (!m_task) {
			return;
		}

		bool isDone = m_task(dt);
		if (isDone) {
			m_task = nullptr;
		}
	}

	bool isDone() const {
		return m_task == nullptr;
	}

private:
	std::function<bool(float)> m_task;
};

class TaskContainer {
public:
	void addTask(Task task) {
		m_tasks.push_back(std::move(task));
	}

	void update(float dt) {
		for (auto it = m_tasks.begin(); it != m_tasks.end();) {
			it->execute(dt);
			if (it->isDone()) {
				it = m_tasks.erase(it);
			} else {
				++it;
			}
		}
	}

	void clear() {
		m_tasks.clear();
	}

	bool isDone() const {
		return m_tasks.empty();
	}

private:
	std::vector<Task> m_tasks;
};
