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
	LevelData* startNextLevel();
	bool isLastLevel() const;

	void update(float dt);

	const LevelData& getCurrentLevel() const {
		return m_currentLevel;
	}

	void setLevelSequence(std::vector<std::string> levelSequence);

	CallbackHandle onGameActionRequest(std::function<void(const GameAction&)> callback);

private:
	void updateTimeline(float dt);
	void updateSpawnBursts(float dt);
	void updateWinLoseCondition(float dt);

	void performKeyframeOperation(const KeyframeOperation& operation);
	void performKeyframeOperation(const SpawnEntityOperation& operation);
	void performKeyframeOperation(const SpawnEntityBurstOperation& operation);
	void performKeyframeOperation(const MessageOperation& operation);
	void performKeyframeOperation(const HUDOperation& operation);
	void performKeyframeOperation(const DefenderPickerOperation& operation);
	void performKeyframeOperation(const CheckOperation& operation);
	void performKeyframeOperation(const FlagTimelineOperation& operation);
	void performKeyframeOperation(const EnableDefenderOperation& operation);
	void performKeyframeOperation(const UpdateValidCellOperation& operation);

	bool checkCondition(const BatteryLevelCondition& condition, float dt);
	bool checkCondition(const AllWavesGoneCondition& condition, float dt);

	void triggerSpawnEntity(const ConfigValue<int>& row, const ConfigValue<int>& column, const ConfigValue<std::string>& id, EntityType type, bool enabled = false);

	const Keyframe* getKeyframe(int index);

	struct SpawnOvertimeTracker {
		const SpawnEntityBurstOperation* info;
		int count{};
		float duration{};
		float time{};
	};

	std::vector<std::string> m_levelSequence;
	std::optional<int> m_currentLevelIndex{};
	LevelData m_currentLevel{};
	bool m_lastKeyframeReached{};
	std::vector<SpawnOvertimeTracker> m_spawnBurstTrackers{};
	CallbackRegistry<const GameAction&> m_onGameActionCallbacks;
	const CheckOperation* m_currentCheckOperation{};

	const GameRegistry& m_gameRegistry;
};
