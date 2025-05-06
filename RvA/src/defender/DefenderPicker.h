#pragma once

#include "DefenderTypes.h"

#include <unordered_map>

class Session;
class GameRegistry;

class DefenderPicker {
public:
	DefenderPicker(Session& session, const GameRegistry& gameRegistry);

	void reset();
	void update(float dt);

	const auto& getAvailableDefenders() const {
		return m_pickableItems;
	}

	const auto& getDefender(DefenderType type) const {
		return m_pickableItems.at(type);
	}

	bool canAfford(DefenderType type) const;
	void startCooldown(DefenderType type);

private:
	Session& m_gameSession;
	const GameRegistry& m_gameRegistry;

	struct Item {
		DefenderType type{};
		int cost{};
		float maxCooldown{};
		float currentCooldown{};
	};

	std::unordered_map<DefenderType, Item> m_pickableItems;
};
