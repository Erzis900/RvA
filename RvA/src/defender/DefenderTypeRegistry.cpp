#include "DefenderTypeRegistry.h"

void DefenderTypeRegistry::registerDefender(DefenderTypeInfo defenderTypeInfo) {
	m_defenderTypes.insert({defenderTypeInfo.type, std::move(defenderTypeInfo)});
}

const DefenderTypeInfo* DefenderTypeRegistry::getDefenderInfo(DefenderType type) const {
	auto itr = m_defenderTypes.find(type);
	return (itr != m_defenderTypes.end()) ? &itr->second : nullptr;
}
