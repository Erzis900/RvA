#include "LevelManager.h"

#include "GameRegistry.h"

LevelManager::LevelManager(const GameRegistry& gameRegistry) : m_gameRegistry(gameRegistry) {}

void LevelManager::resetCurrentLevelIndex() {
	m_currentLevelIndex.reset();
	m_currentLevel = {};
}

void LevelManager::startNextLevel() {
	if (!m_currentLevelIndex) {
		m_currentLevelIndex = 0;
	} else {
		(*m_currentLevelIndex)++;
	}

	m_lastKeyframeReached = false;
	m_currentLevel.time = 0.f;
	m_currentLevel.nextKeyframe = 0;
	m_currentLevel.info = m_gameRegistry.getLevel(m_levelSequence[*m_currentLevelIndex]);
}

void LevelManager::update(float dt) {
	if (!m_lastKeyframeReached) {
		m_currentLevel.time += dt;

		auto keyFrame = getKeyframe(m_currentLevel.nextKeyframe);
		while (m_currentLevel.time >= keyFrame->time) {
			m_currentLevel.nextKeyframe++;

			performAction(keyFrame->action);

			keyFrame = getKeyframe(m_currentLevel.nextKeyframe);
			if (!keyFrame) {
				m_lastKeyframeReached = true;
				return;
			}
		}
	}

	for (auto itr = m_spawnBurstTrackers.begin(); itr != m_spawnBurstTrackers.end();) {
		itr->time += dt;

		if (itr->time >= itr->duration) {
			itr->time = 0.f;
			itr->count--;
			triggerSpawnEnemy(itr->info->row, itr->info->column, itr->info->type);
			if (itr->count <= 0) {
				itr = m_spawnBurstTrackers.erase(itr);
			} else {
				++itr;
			}
		} else {
			++itr;
		}
	}
}

void LevelManager::setLevelSequence(std::vector<std::string> levelSequence) {
	m_levelSequence = std::move(levelSequence);
}

CallbackHandle LevelManager::onSpawnWaveRequest(std::function<void()> callback) {
	return m_onSpawnWaveRequest.registerCallback(std::move(callback));
}

CallbackHandle LevelManager::onGameActionRequest(std::function<void(const GameAction&)> callback) {
	return m_onGameActionCallbacks.registerCallback(std::move(callback));
}

CallbackHandle LevelManager::onVictoryConditionFulfilled(std::function<void()> callback) {
	return m_onVictoryConditionFullfilled.registerCallback(std::move(callback));
}

CallbackHandle LevelManager::onLosingConditionFulfilled(std::function<void()> callback) {
	return m_onLosingConditionFullfilled.registerCallback(std::move(callback));
}

void LevelManager::performAction(const KeyframeAction& action) {
	std::visit([this](auto&& arg) { performAction(arg); }, action);
}

void LevelManager::performAction(const SpawnEnemy& action) {
	triggerSpawnEnemy(action.row, action.column, action.type);
}

void LevelManager::performAction(const SpawnEnemyBurst& action) {
	m_spawnBurstTrackers.emplace_back(SpawnOvertimeTracker{.info = &action, .count = action.amount.generate(), .duration = action.interval.generate(), .time = 0.f});
}

void LevelManager::triggerSpawnEnemy(const ConfigValue<int>& row, const ConfigValue<int>& column, const ConfigValue<std::string>& type) {
	auto enemyType = type.generate();
	auto rowVal = row.generate();
	auto columnVal = column.generate();
	m_onGameActionCallbacks.executeCallbacks(EnemySpawnAction{enemyType, rowVal, columnVal});
}

const Keyframe* LevelManager::getKeyframe(int index) {
	if (index < 0 || index >= m_currentLevel.info->timeline.keyframes.size()) {
		return nullptr;
	}
	return &m_currentLevel.info->timeline.keyframes[index];
}
