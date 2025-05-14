#include "DefenderPicker.h"

#include "GameRegistry.h"
#include "Session.h"

DefenderPicker::DefenderPicker(Session& session, const GameRegistry& gameRegistry) : m_gameSession(session), m_gameRegistry(gameRegistry) {}

void DefenderPicker::reset() {
	m_pickableItems.clear();
}

void DefenderPicker::update(float dt) {
	for (auto& item : m_pickableItems) {
		item.currentCooldown -= dt;
		if (item.currentCooldown < 0.f) {
			item.currentCooldown = 0.f;
		}
	}
}

bool DefenderPicker::canAfford(const std::string& id) const {
	const auto& defenderInfo = m_gameRegistry.getDefender(id);
	assert(defenderInfo);
	if (!defenderInfo) {
		return false;
	}
	return m_gameSession.getCurrentLevel().scraps >= defenderInfo->cost;
}

void DefenderPicker::startCooldown(const std::string& id) {
	auto it = std::ranges::find_if(m_pickableItems, [&](const auto& item) { return item.id == id; });
	assert(it != m_pickableItems.end());
	if (it != m_pickableItems.end()) {
		it->currentCooldown = it->maxCooldown;
	}
}

void DefenderPicker::addPickableItem(const std::string& id) {
	const auto& defenderInfo = m_gameRegistry.getDefender(id);
	m_pickableItems.emplace_back(id, defenderInfo->cost, defenderInfo->buildCooldown);
}
