#include "EnemyManager.h"

#include "GameRegistry.h"
#include "MusicManager.h"
#include "constants.h"

#include <raylib.h>
#include <raymath.h>

EnemyManager::EnemyManager(const GameRegistry& gameRegistry, CollisionSystem& collisionSystem, MusicManager& musicManager)
	: m_gameRegistry(gameRegistry)
	, m_collisionSystem(collisionSystem)
	, m_musicManager(musicManager) {}

void EnemyManager::clear() {
	for (auto& enemy : m_enemies) {
		m_collisionSystem.destroyCollider(enemy->getColliderHandle());
	}
	m_enemies.clear();
	m_enemyDestroyedInfos.reserve(32);
}

GameActions EnemyManager::update(float dt) {
	GameActions actions;

	m_enemyDestroyedInfos.clear();
	for (auto it = m_enemies.begin(); it != m_enemies.end();) {
		auto& enemy = *it;

		auto gameAction = enemy->update(dt);
		if (!std::holds_alternative<std::monostate>(gameAction)) {
			actions.push_back(gameAction);
		}

		if (!enemy->isDying()) {
			if (enemy->getHp() <= 0) {
				enemy->setState(EnemyState::Dying);
				m_musicManager.playSound("alien_death");
				m_collisionSystem.destroyCollider(enemy->getColliderHandle());
			} else {
				m_collisionSystem.updateCollider(enemy->getColliderHandle(), enemy->calculateBoundingBox());
			}
		}

		if (enemy->getState() == EnemyState::Dead) {
			m_enemyDestroyedInfos.emplace_back(enemy->getInfo(), enemy->getCenteredPosition(), enemy->getLatestDamageApplied().source);
			it = m_enemies.erase(it);
		} else {
			++it;
		}
	}

	if (!m_enemyDestroyedInfos.empty()) {
		m_onEnemiesDestroyedCallbacks.executeCallbacks(m_enemyDestroyedInfos);
	}

	return actions;
}

void EnemyManager::draw(Atlas& atlas) {
	for (auto& enemy : m_enemies) {
		enemy->draw(atlas);
	}
}

Enemy* EnemyManager::findClosestEnemy(const Vector2& position, bool filterDead) {
	Enemy* closestEnemy = nullptr;
	float closestDistSq = std::numeric_limits<float>::max();

	for (auto& enemy : m_enemies) {
		if (!filterDead || enemy->isDying() != filterDead) {
			float distSq = Vector2LengthSqr(Vector2Subtract(enemy->getCenteredPosition(), position));
			if (distSq < closestDistSq) {
				closestDistSq = distSq;
				closestEnemy = enemy.get();
			}
		}
	}

	return closestEnemy;
}

void EnemyManager::spawnEnemy(const EnemyTypeInfo* info, int row, int column) {
	Vector2 position = getSnappedPosition(row, column);
	position.y -= 5;

	auto enemy = std::make_unique<Enemy>(position, info, row);
	enemy->setColliderHandle(m_collisionSystem.createCollider(Collider::Flag::Enemy, enemy.get()));
	m_enemies.push_back(std::move(enemy));
}

CallbackHandle EnemyManager::onEnemiesDestroyed(std::function<void(const std::vector<EnemyDestroyedInfo>&)> callback) {
	return m_onEnemiesDestroyedCallbacks.registerCallback(std::move(callback));
}
