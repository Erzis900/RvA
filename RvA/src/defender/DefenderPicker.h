#pragma once

#include "DefenderTypes.h"

#include <cassert>
#include <string>
#include <unordered_map>

class Session;
class GameRegistry;

class DefenderPicker {
public:
	DefenderPicker(Session& session, const GameRegistry& gameRegistry);

	void reset();
	void resetCooldowns();
	void update(float dt);

	const auto& getAvailableDefenders() const {
		return m_pickableItems;
	}

	const auto& getDefender(const std::string& id) const {
		auto it = std::ranges::find_if(m_pickableItems, [&](const auto& item) { return item.id == id; });
		assert(it != m_pickableItems.end());
		return *it;
	}

	bool canAfford(const std::string& id) const;
	void startCooldown(const std::string& id);

	void addPickableItem(const std::string& id);

private:
	Session& m_gameSession;
	const GameRegistry& m_gameRegistry;

	struct Item {
		std::string id{};
		int cost{};
		float maxCooldown{};
		float currentCooldown{};
	};

	std::vector<Item> m_pickableItems;
};
