#include "GameRegistry.h"

void GameRegistry::addEnemy(std::string id, EnemyTypeInfo info) {
	m_enemyRegistry.registerEnemyType(std::move(id), std::move(info));
}

void GameRegistry::addDefender(DefenderTypeInfo info) {
	m_defenderRegistry.registerDefender(std::move(info));
}

void GameRegistry::addBullet(std::string id, BulletData info) {
	m_bulletRegistry.registerBulletType(std::move(id), std::move(info));
}

void GameRegistry::addDrop(std::string id, DropTypeInfo info) {
	m_dropRegistry.addDrop(std::move(id), std::move(info));
}

void GameRegistry::addLevel(std::string id, LevelInfo info) {
	m_levelRegistry.registerLevel(std::move(id), std::move(info));
}

const EnemyTypeInfo* GameRegistry::getEnemy(const std::string& id) const {
	return m_enemyRegistry.getEnemyTypeInfo(id);
}

const DefenderTypeInfo* GameRegistry::getDefender(DefenderType type) const {
	return m_defenderRegistry.getDefenderInfo(type);
}

const BulletData* GameRegistry::getBullet(const std::string& id) const {
	return m_bulletRegistry.getBulletInfo(id);
}

const DropTypeInfo* GameRegistry::getDrop(const std::string& id) const {
	return m_dropRegistry.getDropType(id);
}

const LevelInfo* GameRegistry::getLevel(const std::string& id) const {
	return m_levelRegistry.getLevel(id);
}
