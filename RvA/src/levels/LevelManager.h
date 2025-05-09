#pragma once

#include "GameAction.h"
#include "LevelData.h"
#include "utilities/CallbackRegistry.h"

#include <optional>
#include <string>
#include <vector>

class GameRegistry;

/*
 * The LevelManager class is responsible for updating the state of the current level
 * It tells when to spawn new waves, when to spawn new enemies, and when the player win or lose
 */
class LevelManager {
public:
	LevelManager(const GameRegistry& gameRegistry);

	void resetCurrentLevelIndex();
	void startNextLevel();

	void update(float dt);

	void setLevelSequence(std::vector<std::string> levelSequence);

	CallbackHandle onSpawnWaveRequest(std::function<void()> callback);
	CallbackHandle onGameActionRequest(std::function<void(const GameAction&)> callback);
	CallbackHandle onVictoryConditionFulfilled(std::function<void()> callback);
	CallbackHandle onLosingConditionFulfilled(std::function<void()> callback);

private:
	void performAction(const KeyframeAction& action);
	void performAction(const SpawnEnemy& action);
	void performAction(const SpawnEnemyBurst& action);

	void triggerSpawnEnemy(const ConfigValue<int>& row, const ConfigValue<int>& column, const ConfigValue<std::string>& type);

	const Keyframe* getKeyframe(int index);

	struct SpawnOvertimeTracker {
		const SpawnEnemyBurst* info;
		int count{};
		float duration{};
		float time{};
	};

	std::vector<std::string> m_levelSequence;
	std::optional<int> m_currentLevelIndex{};
	LevelData m_currentLevel{};
	bool m_lastKeyframeReached{};
	std::vector<SpawnOvertimeTracker> m_spawnBurstTrackers{};
	CallbackRegistry<> m_onLosingConditionFullfilled;
	CallbackRegistry<> m_onVictoryConditionFullfilled;
	CallbackRegistry<> m_onSpawnWaveRequest;
	CallbackRegistry<const GameAction&> m_onGameActionCallbacks;

	const GameRegistry& m_gameRegistry;
};
