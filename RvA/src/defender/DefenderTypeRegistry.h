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
	std::string id;
	std::string name;
	AnimationData spriteEnabled;
	AnimationData spriteDisabled;
	AnimationData spriteShoot;
	AnimationData spriteDying;
	float batteryDrain{};
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
	void registerDefender(std::string id, DefenderTypeInfo defenderTypeInfo);
	const DefenderTypeInfo* getDefenderInfo(const std::string& id) const;

	auto& getDefenderInfos() const {
		return m_defenderTypes;
	}

private:
	std::unordered_map<std::string, DefenderTypeInfo> m_defenderTypes;
};
