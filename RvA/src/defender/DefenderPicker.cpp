#include "DefenderPicker.h"

#include "GameRegistry.h"
#include "Session.h"

DefenderPicker::DefenderPicker(Session& session, const GameRegistry& gameRegistry) : m_gameSession(session), m_gameRegistry(gameRegistry) {}

void DefenderPicker::reset() {
	const auto& defenderTypeInfos = m_gameRegistry.getDefenders();

	m_pickableItems.clear();
	for (auto& [id, info] : defenderTypeInfos) {
		m_pickableItems.emplace(id, Item{.id = id, .cost = info.cost, .maxCooldown = info.buildCooldown});
	}
}

void DefenderPicker::update(float dt) {
	for (auto& [type, item] : m_pickableItems) {
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
	auto it = m_pickableItems.find(id);
	assert(it != m_pickableItems.end());
	if (it != m_pickableItems.end()) {
		it->second.currentCooldown = it->second.maxCooldown;
	}
}
