#pragma once

#include "DefenderTypes.h"

#include <string>
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

	const auto& getDefender(const std::string& id) const {
		return m_pickableItems.at(id);
	}

	bool canAfford(const std::string& id) const;
	void startCooldown(const std::string& id);

private:
	Session& m_gameSession;
	const GameRegistry& m_gameRegistry;

	struct Item {
		std::string id{};
		int cost{};
		float maxCooldown{};
		float currentCooldown{};
	};

	std::unordered_map<std::string, Item> m_pickableItems;
};
