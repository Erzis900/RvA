#pragma once

#include "DefenderTypes.h"

#include <unordered_map>

class Session;
class DefenderTypeRegistry;

class DefenderPicker {
public:
	DefenderPicker(Session& session, const DefenderTypeRegistry& defenderTypeRegistry);

	void reset();
	void update(float dt);

	bool canAfford(DefenderType type) const;
	float getCurrentCooldown(DefenderType type) const;
	void startCooldown(DefenderType type);

private:
	Session& m_gameSession;
	const DefenderTypeRegistry& m_defenderTypeRegistry;

	struct Item {
		DefenderType type{};
		int cost{};
		float maxCooldown{};
		float currentCooldown{};
	};

	std::unordered_map<DefenderType, Item> m_pickableItems;
};
