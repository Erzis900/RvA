#pragma once

#include "Animation.h"
#include "defender/DefenderTypes.h"

#include <optional>
#include <unordered_map>

/*
 * The DefenderTypeInfo gives us information about the type of defender
 * For example what's the max number of HP or how much it cost, and so on.
 */
struct DefenderTypeInfo {
	DefenderType type{};
	std::string name;
	AnimationData spriteEnabled;
	AnimationData spriteDisabled;
	AnimationData spriteShoot;
	AnimationData spriteDying;
	float batteryDrain{};
	float scrapsGain{};
	float firstShootCooldown{};
	float shootCooldown{};
	int maxHP{};
	int cost{};
	std::optional<std::string> bulletType;
	float shootingAnimationTime{};
	float buildCooldown{};
};

class DefenderTypeRegistry {
public:
	void registerDefender(DefenderTypeInfo defenderTypeInfo);
	const DefenderTypeInfo* getDefenderInfo(DefenderType type) const;

	auto& getDefenderInfos() const {
		return m_defenderTypes;
	}

private:
	std::unordered_map<DefenderType, DefenderTypeInfo> m_defenderTypes;
};
