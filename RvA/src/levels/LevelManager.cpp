#include "LevelManager.h"

#include "GameRegistry.h"

LevelManager::LevelManager(const GameRegistry& gameRegistry) : m_gameRegistry(gameRegistry) {}

void LevelManager::resetCurrentLevelIndex() {
	m_currentLevelIndex.reset();
	m_currentLevel = {};
}

LevelData* LevelManager::startNextLevel() {
	if (!m_currentLevelIndex) {
		m_currentLevelIndex = 0;
	} else {
		(*m_currentLevelIndex)++;
	}

	m_lastKeyframeReached = false;
	m_currentLevel.time = 0.f;
	m_currentLevel.nextKeyframe = 0;
	m_currentLevel.info = m_gameRegistry.getLevel(m_levelSequence[*m_currentLevelIndex]);
	m_currentLevel.scraps = m_currentLevel.info->startingScraps;
	m_currentLevel.batteryCharge = m_currentLevel.info->maxBatteryCharge;
	m_currentLevel.enemyCount = 0;
	m_currentLevel.isWinningCountdownActive = false;
	m_currentLevel.countdownToWin = 0.f;
	m_spawnBurstTrackers.clear();
	return &m_currentLevel;
}

bool LevelManager::isLastLevel() const {
	if (!m_currentLevelIndex) {
		return false;
	}
	return *m_currentLevelIndex >= static_cast<int>(m_levelSequence.size()) - 1;
}

void LevelManager::update(float dt) {
	updateTimeline(dt);
	updateSpawnBursts(dt);
	updateWinLoseCondition(dt);
}

void LevelManager::updateTimeline(float dt) {
	if (!m_lastKeyframeReached) {
		m_currentLevel.time += dt;

		auto keyFrame = getKeyframe(m_currentLevel.nextKeyframe);
		while (m_currentLevel.time >= keyFrame->time) {
			m_currentLevel.nextKeyframe++;

			performKeyframeOperation(keyFrame->action);

			keyFrame = getKeyframe(m_currentLevel.nextKeyframe);
			if (!keyFrame) {
				m_lastKeyframeReached = true;
				return;
			}
		}
	}
}

void LevelManager::updateSpawnBursts(float dt) {
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

void LevelManager::performKeyframeOperation(const KeyframeOperation& action) {
	std::visit([this](auto&& arg) { performKeyframeOperation(arg); }, action);
}

void LevelManager::performKeyframeOperation(const SpawnEnemyOperation& action) {
	triggerSpawnEnemy(action.row, action.column, action.type);
}

void LevelManager::performKeyframeOperation(const SpawnEnemyBurstOperation& action) {
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

void LevelManager::updateWinLoseCondition(float dt) {
	auto win = std::visit([this, dt](auto&& condition) { return checkCondition(condition, dt); }, m_currentLevel.info->winCondition);
	auto lost = std::visit([this, dt](auto&& condition) { return checkCondition(condition, dt); }, m_currentLevel.info->loseCondition);

	// Check if the win condition is satisfied
	if (win) {
		if (!m_currentLevel.isWinningCountdownActive) {
			// If the countdown to win is not active, we start it
			m_currentLevel.isWinningCountdownActive = true;
			m_currentLevel.countdownToWin = m_currentLevel.info->winCountdownDuration;
		} else {
			// If the countdown to win is active, we decrease it
			m_currentLevel.countdownToWin -= dt;
			if (m_currentLevel.countdownToWin <= 0.f) {
				m_onGameActionCallbacks.executeCallbacks(WinAction{});
			}
		}
	} else if (m_currentLevel.isWinningCountdownActive) {
		// If the countdown to win was active and the Winning condition is nomore satisfied, we reset the countdown
		m_currentLevel.isWinningCountdownActive = false;
		m_currentLevel.countdownToWin = 0.f;
	}

	if (lost) {
		m_onGameActionCallbacks.executeCallbacks(LoseAction{});
	}
}

bool LevelManager::checkCondition(const BatteryLevelCondition& condition, float dt) {
	auto batteryLevel = m_currentLevel.batteryCharge;
	return std::visit([batteryLevel](ConfigCondition<float>&& cond) { return cond.check(batteryLevel); }, condition.batteryLevel);
}

bool LevelManager::checkCondition(const AllWavesGoneCondition& condition, float dt) {
	return m_lastKeyframeReached && m_spawnBurstTrackers.empty() && m_currentLevel.enemyCount == 0;
}

void LevelManager::setLevelSequence(std::vector<std::string> levelSequence) {
	m_levelSequence = std::move(levelSequence);
}

CallbackHandle LevelManager::onGameActionRequest(std::function<void(const GameAction&)> callback) {
	return m_onGameActionCallbacks.registerCallback(std::move(callback));
}
