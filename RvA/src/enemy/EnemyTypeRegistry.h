#pragma once

#include "Animation.h"
#include "enemy/EnemyTypes.h"
#include "utilities/ConfigValue.h"

#include <optional>
#include <unordered_map>

/*
 * Enemy Behaviors
 */

struct PortalSpawnBehaviorInfo {
	float timeBeforeActing{};			// How long the enemy waits before deciding if it should spawn a portal
	float timeBeforeActingAgain{};		// How long the enemy waits before deciding if it should spawn a portal again
	float chanceIncreaseOverTime{};		// How much the chance to actually spawn a portal increases over time
	float baseChanceToSpawn{};			// The base chance to spawn a portal
	ConfigValue<int> columnDistance{};	// The distance between the entrance and exit portal
	ConfigValue<int> rowDistance{};		// The distance between the entrance and exit portal
	ConfigValue<int> portalCastRange{}; // The range in which the enemy can spawn a entrance portal
	AnimationData animation;			// The animation to play when the enemy spawns a portal
};

struct PortalSpawnBehavior {
	float currentChance{};		  // The current chance to spawn a portal
	float timeChance{};			  // Time counter to increase the chance to spawn a portal
	int numberOfSpawnedPortals{}; // The number of portals spawned by this enemy
};

using EnemyBehaviorInfo = std::variant<std::monostate, PortalSpawnBehaviorInfo>; // Static data for the enemy behavior
using EnemyBehavior = std::variant<std::monostate, PortalSpawnBehavior>;		 // Dynamic data for the enemy behavior

/*
 * Enemy Info
 */

struct EnemyTypeInfo {
	EnemyType type{};
	float maxHp{};
	float speed{};
	float attackTime{};
	float defenderDamage{};
	float baseWallDamage{};
	float bounceResistance{};
	std::optional<std::string> dropType{};
	ConfigValue<int> dropAmount{};
	AnimationData idleAnimation;
	AnimationData moveAnimation;
	AnimationData attackAnimation;
	AnimationData dyingAnimation;
	AnimationData summonAnimation;
	AnimationData sparkEffect;
	EnemyBehaviorInfo behavior{}; // right now we consider only one potential behaviour per enemy
};

class EnemyTypeRegistry {
public:
	void registerEnemyType(std::string id, EnemyTypeInfo typeInfo);
	const EnemyTypeInfo* getEnemyTypeInfo(const std::string& id) const;

	const auto& getEnemyTypeInfos() const {
		return m_enemyTypes;
	};

private:
	std::unordered_map<std::string, EnemyTypeInfo> m_enemyTypes;
};
