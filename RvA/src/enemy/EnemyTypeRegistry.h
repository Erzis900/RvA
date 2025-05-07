#pragma once

#include "Animation.h"
#include "enemy/EnemyTypes.h"
#include "utilities/ConfigValue.h"

#include <optional>
#include <unordered_map>

struct EnemyTypeInfo {
	EnemyType type{};
	float spawnChance{};
	float maxHp{};
	float speed{};
	float attackTime{};
	float defenderDamage{};
	float baseWallDamage{};
	std::optional<std::string> dropType{};
	ConfigValue<int> dropAmount{};
	AnimationData idleAnimation;
	AnimationData moveAnimation;
	AnimationData attackAnimation;
	AnimationData dyingAnimation;
};

class EnemyTypeRegistry {
public:
	void registerEnemyType(EnemyTypeInfo typeInfo);
	const EnemyTypeInfo* getEnemyTypeInfo(EnemyType type) const;

	const auto& getEnemyTypeInfos() const {
		return m_enemyTypes;
	};

private:
	std::unordered_map<EnemyType, EnemyTypeInfo> m_enemyTypes;
};
