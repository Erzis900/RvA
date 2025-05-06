#include "EnemyTypeRegistry.h"

void EnemyTypeRegistry::registerEnemyType(EnemyTypeInfo typeInfo) {
	m_enemyTypes.insert({typeInfo.type, std::move(typeInfo)});
}

const EnemyTypeInfo* EnemyTypeRegistry::getEnemyTypeInfo(EnemyType type) const {
	auto itr = m_enemyTypes.find(type);
	return (itr != m_enemyTypes.end()) ? &itr->second : nullptr;
}
