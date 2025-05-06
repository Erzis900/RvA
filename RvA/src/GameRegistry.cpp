#include "GameRegistry.h"

void GameRegistry::addEnemy(EnemyTypeInfo info) {
	m_enemyRegistry.registerEnemyType(std::move(info));
}

void GameRegistry::addDefender(DefenderTypeInfo info) {
	m_defenderRegistry.registerDefender(std::move(info));
}

void GameRegistry::addBullet(std::string id, BulletData info) {
	m_bulletRegistry.registerBulletType(std::move(id), std::move(info));
}

const EnemyTypeInfo* GameRegistry::getEnemy(EnemyType type) const {
	return m_enemyRegistry.getEnemyTypeInfo(type);
}

const DefenderTypeInfo* GameRegistry::getDefender(DefenderType type) const {
	return m_defenderRegistry.getDefenderInfo(type);
}

const BulletData* GameRegistry::getBullet(const std::string& id) const {
	return m_bulletRegistry.getBulletInfo(id);
}
