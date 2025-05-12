#include "DefenderTypeRegistry.h"

void DefenderTypeRegistry::registerDefender(std::string id, DefenderTypeInfo defenderTypeInfo) {
	m_defenderTypes.insert({std::move(id), std::move(defenderTypeInfo)});
}

const DefenderTypeInfo* DefenderTypeRegistry::getDefenderInfo(const std::string& id) const {
	auto itr = m_defenderTypes.find(id);
	return (itr != m_defenderTypes.end()) ? &itr->second : nullptr;
}
