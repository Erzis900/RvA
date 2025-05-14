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
		// If we have a current check operation we check it first
		// If the check returns false we don't update the timeline
		// This is useful for example when we have a check that checks if
		// the player has enough scraps to spawn a defender or similar waiting mechanics
		if (m_currentCheckOperation) {
			if (m_currentCheckOperation->check(m_currentLevel)) {
				m_currentCheckOperation = nullptr;
			} else {
				return;
			}
		}

		m_currentLevel.time += dt;

		auto keyFrame = getKeyframe(m_currentLevel.nextKeyframe);
		while (m_currentLevel.time >= keyFrame->time) {
			m_currentLevel.nextKeyframe++;

			performKeyframeOperation(keyFrame->action);
			if (!m_currentCheckOperation) {
				keyFrame = getKeyframe(m_currentLevel.nextKeyframe);
				if (!keyFrame) {
					m_lastKeyframeReached = true;
					return;
				}
			} else {
				break;
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
			triggerSpawnEntity(itr->info->row, itr->info->column, itr->info->id, itr->info->type);
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

void LevelManager::performKeyframeOperation(const KeyframeOperation& operation) {
	std::visit([this](auto&& arg) { performKeyframeOperation(arg); }, operation);
}

void LevelManager::performKeyframeOperation(const SpawnEntityOperation& operation) {
	triggerSpawnEntity(operation.row, operation.column, operation.id, operation.type, operation.enabled);
}

void LevelManager::performKeyframeOperation(const SpawnEntityBurstOperation& operation) {
	m_spawnBurstTrackers.emplace_back(SpawnOvertimeTracker{.info = &operation, .count = operation.amount.generate(), .duration = operation.interval.generate(), .time = 0.f});
}

void LevelManager::performKeyframeOperation(const TutorialOperation& operation) {
	m_onGameActionCallbacks.executeCallbacks(operation);
}

void LevelManager::performKeyframeOperation(const HUDOperation& operation) {
	m_onGameActionCallbacks.executeCallbacks(HUDAction{.type = operation.type});
}

void LevelManager::performKeyframeOperation(const DefenderPickerOperation& operation) {
	m_onGameActionCallbacks.executeCallbacks(DefenderPickerAction{.type = operation.type, .id = operation.id});
}

void LevelManager::performKeyframeOperation(const CheckOperation& operation) {
	if (!operation.check(m_currentLevel)) {
		m_currentCheckOperation = &operation; // If the check is false we store the operation to be performed every update
	}
}

void LevelManager::triggerSpawnEntity(const ConfigValue<int>& row, const ConfigValue<int>& column, const ConfigValue<std::string>& id, EntityType type, bool enabled) {
	auto entityId = id.generate();
	auto rowVal = row.generate();
	auto columnVal = column.generate();
	if (type == EntityType::Defender) {
		m_onGameActionCallbacks.executeCallbacks(DefenderSpawnAction{entityId, rowVal, columnVal, enabled});
	} else if (type == EntityType::Enemy) {
		m_onGameActionCallbacks.executeCallbacks(EnemySpawnAction{entityId, rowVal, columnVal});
	}
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
