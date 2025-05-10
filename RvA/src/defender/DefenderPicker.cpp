#include "DefenderPicker.h"

#include "GameRegistry.h"
#include "Session.h"

DefenderPicker::DefenderPicker(Session& session, const GameRegistry& gameRegistry) : m_gameSession(session), m_gameRegistry(gameRegistry) {}

void DefenderPicker::reset() {
	const auto& defenderTypeInfos = m_gameRegistry.getDefenders();

	m_pickableItems.clear();
	for (auto& [type, info] : defenderTypeInfos) {
		m_pickableItems.emplace(type, Item{.type = type, .cost = info.cost, .maxCooldown = info.buildCooldown});
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

bool DefenderPicker::canAfford(DefenderType type) const {
	const auto& defenderInfo = m_gameRegistry.getDefender(type);
	assert(defenderInfo);
	if (!defenderInfo) {
		return false;
	}
	return m_gameSession.getCurrentLevel().scraps >= defenderInfo->cost;
}

void DefenderPicker::startCooldown(DefenderType type) {
	auto it = m_pickableItems.find(type);
	assert(it != m_pickableItems.end());
	if (it != m_pickableItems.end()) {
		it->second.currentCooldown = it->second.maxCooldown;
	}
}
