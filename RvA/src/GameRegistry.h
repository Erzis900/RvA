#pragma once

#include "bullet/BulletTypeRegistry.h"
#include "defender/DefenderTypeRegistry.h"
#include "drops/DropRegistry.h"
#include "enemy/EnemyTypeRegistry.h"
#include "levels/LevelRegistry.h"

/*
 * A class that keeps track of all the static data used as configuration for game.
 * It includes all the info like defenders, enemies, and bullet stats.
 */
class GameRegistry {
public:
	void addEnemy(std::string id, EnemyTypeInfo info);
	void addDefender(DefenderTypeInfo info);
	void addBullet(std::string id, BulletData info);
	void addDrop(std::string id, DropTypeInfo info);
	void addLevel(std::string id, LevelInfo info);

	const EnemyTypeInfo* getEnemy(const std::string& id) const;
	const DefenderTypeInfo* getDefender(DefenderType type) const;
	const BulletData* getBullet(const std::string& id) const;
	const DropTypeInfo* getDrop(const std::string& id) const;
	const LevelInfo* getLevel(const std::string& id) const;

	const auto& getEnemies() const {
		return m_enemyRegistry.getEnemyTypeInfos();
	}

	const auto& getDefenders() const {
		return m_defenderRegistry.getDefenderInfos();
	}

private:
	EnemyTypeRegistry m_enemyRegistry;
	DefenderTypeRegistry m_defenderRegistry;
	BulletTypeRegistry m_bulletRegistry;
	DropRegistry m_dropRegistry;
	LevelRegistry m_levelRegistry;
};
