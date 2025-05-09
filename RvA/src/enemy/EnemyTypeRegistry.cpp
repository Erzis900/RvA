#include "EnemyTypeRegistry.h"

void EnemyTypeRegistry::registerEnemyType(std::string id, EnemyTypeInfo typeInfo) {
	m_enemyTypes.insert({std::move(id), std::move(typeInfo)});
}

const EnemyTypeInfo* EnemyTypeRegistry::getEnemyTypeInfo(const std::string& id) const {
	auto itr = m_enemyTypes.find(id);
	return (itr != m_enemyTypes.end()) ? &itr->second : nullptr;
}
