#include "GameRegistry.h"

void GameRegistry::addEnemy(std::string id, EnemyTypeInfo info) {
	m_enemyRegistry.registerEnemyType(std::move(id), std::move(info));
}

void GameRegistry::addEnemyFromTemplate(const std::string& templateId, std::string id, std::function<void(EnemyTypeInfo&)> callback) {
	auto enemyTypeInfo = m_enemyRegistry.getEnemyTypeInfo(templateId);
	if (enemyTypeInfo) {
		EnemyTypeInfo newEnemyTypeInfo = *enemyTypeInfo;
		callback(newEnemyTypeInfo);
		addEnemy(std::move(id), std::move(newEnemyTypeInfo));
	} else {
		assert(0 && "Enemy type not found");
	}
}

void GameRegistry::addDefender(std::string id, DefenderTypeInfo info) {
	m_defenderRegistry.registerDefender(std::move(id), std::move(info));
}

void GameRegistry::addDefenderFromTemplate(const std::string& templateId, std::string id, std::function<void(DefenderTypeInfo&)> callback) {
	auto defenderTypeInfo = m_defenderRegistry.getDefenderInfo(templateId);
	if (defenderTypeInfo) {
		DefenderTypeInfo newDefenderTypeInfo = *defenderTypeInfo;
		callback(newDefenderTypeInfo);
		addDefender(std::move(id), std::move(newDefenderTypeInfo));
	} else {
		assert(0 && "Defender type not found");
	}
}

void GameRegistry::addBullet(std::string id, BulletData info) {
	m_bulletRegistry.registerBulletType(std::move(id), std::move(info));
}

void GameRegistry::addDrop(std::string id, DropTypeInfo info) {
	m_dropRegistry.addDrop(std::move(id), std::move(info));
}

void GameRegistry::addDropFromTemplate(const std::string& templateId, std::string id, std::function<void(DropTypeInfo&)> callback) {
	auto dropTypeInfo = m_dropRegistry.getDropType(templateId);
	if (dropTypeInfo) {
		DropTypeInfo newDropTypeInfo = *dropTypeInfo;
		callback(newDropTypeInfo);
		addDrop(std::move(id), std::move(newDropTypeInfo));
	} else {
		assert(0 && "Drop type not found");
	}
}

void GameRegistry::addLevel(std::string id, LevelInfo info) {
	info.id = id;
	m_levelRegistry.registerLevel(std::move(id), std::move(info));
}

void GameRegistry::addLevelFromTemplate(const std::string& templateId, std::string id, std::function<void(LevelInfo&)> callback) {
	auto levelInfo = m_levelRegistry.getLevel(templateId);
	if (levelInfo) {
		LevelInfo newLevelInfo = *levelInfo;
		callback(newLevelInfo);
		addLevel(std::move(id), std::move(newLevelInfo));
	} else {
		assert(0 && "Level type not found");
	}
}

void GameRegistry::addPortal(std::string id, PortalTypeInfo info) {
	m_portalRegistry.registerPortal(std::move(id), std::move(info));
}

void GameRegistry::addPortalFromTemplate(const std::string& templateId, std::string id, std::function<void(PortalTypeInfo&)> callback) {
	auto portalTypeInfo = m_portalRegistry.getPortalInfo(templateId);
	if (portalTypeInfo) {
		PortalTypeInfo newPortalTypeInfo = *portalTypeInfo;
		callback(newPortalTypeInfo);
		addPortal(std::move(id), std::move(newPortalTypeInfo));
	} else {
		assert(0 && "Portal type not found");
	}
}

const EnemyTypeInfo* GameRegistry::getEnemy(const std::string& id) const {
	return m_enemyRegistry.getEnemyTypeInfo(id);
}

const DefenderTypeInfo* GameRegistry::getDefender(const std::string& id) const {
	return m_defenderRegistry.getDefenderInfo(id);
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

const PortalTypeInfo* GameRegistry::getPortal(const std::string& id) const {
	return m_portalRegistry.getPortalInfo(id);
}

void verifyLevelCoordinate(int min, int max, const ConfigValue<int>& value) {
	std::visit(
		[&](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, FixedValue<int>>) {
				assert(arg.value >= min && arg.value < max);
			} else if constexpr (std::is_same_v<T, RandomRange<int>>) {
				assert(arg.min >= min && arg.min < arg.max && arg.max >= min && arg.max < max);
			} else if constexpr (std::is_same_v<T, RandomRangeStep<int>>) {
				assert(arg.min >= min && arg.min < arg.max && arg.max >= min && arg.max < max);
			} else if constexpr (std::is_same_v<T, Selection<int>>) {
				for (const auto& weightedValue : arg) {
					assert(weightedValue.value >= min && weightedValue.value < max);
				}
			} else {
				static_assert(sizeof(T) == 0, "Non-exhaustive visitor!");
			}
		},
		value);
}

void GameRegistry::verifyData() {
	auto& levels = getLevels();
	for (const auto& [levelName, level] : levels) {
		// verify that the keyframes are defined in the right order
		for (size_t i = 0; i < level.timeline.keyframes.size(); ++i) {
			if (i != 0 && level.timeline.keyframes[i].time < level.timeline.keyframes[i - 1].time) {
				assert(0 && "Keyframes are not in the right order");
			}

			// verify that the SpawnEnemyOperation and SpawnEnemyOperationBurst are indicating valid rows and columns
			if (std::holds_alternative<SpawnEntityOperation>(level.timeline.keyframes[i].action)) {
				auto& spawnEnemyOp = std::get<SpawnEntityOperation>(level.timeline.keyframes[i].action);
				verifyLevelCoordinate(0, ROWS, spawnEnemyOp.row);
				verifyLevelCoordinate(0, COLS + 1, spawnEnemyOp.column);
			} else if (std::holds_alternative<SpawnEntityBurstOperation>(level.timeline.keyframes[i].action)) {
				auto& spawnBurstOp = std::get<SpawnEntityBurstOperation>(level.timeline.keyframes[i].action);
				verifyLevelCoordinate(0, ROWS, spawnBurstOp.row);
				verifyLevelCoordinate(0, COLS + 1, spawnBurstOp.column);
			}
		}
	}
}
