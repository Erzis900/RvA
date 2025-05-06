#include "DefenderPicker.h"

#include "Session.h"

DefenderPicker::DefenderPicker(Session& session, const DefenderTypeRegistry& defenderTypeRegistry) : m_gameSession(session), m_defenderTypeRegistry(defenderTypeRegistry) {}

void DefenderPicker::update(float dt) {}

bool DefenderPicker::canAfford(DefenderType type) const {
	const auto& defenderInfo = m_gameSession.getDefenderTypeRegistry().getDefenderInfo(type);
	if (!defenderInfo) {
		return false;
	}
	return m_gameSession.getScraps() >= defenderInfo->cost;
}

bool DefenderPicker::isInCooldown(DefenderType type) const {
	return false;
}

void DefenderPicker::startCooldown(DefenderType type) {}
