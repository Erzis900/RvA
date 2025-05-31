#include "DefenderManager.h"

#include "MusicManager.h"
#include "ResourceSystem.h"
#include "collisions/CollisionSystem.h"
#include "enemy/EnemyManager.h"

DefenderManager::DefenderManager(CollisionSystem& collisionSystem, MusicManager& musicManager, ResourceSystem& resourceSystem, EnemyManager& enemyManager)
	: m_collisionSystem(collisionSystem)
	, m_musicManager(musicManager)
	, m_resourceSystem(resourceSystem)
	, m_enemyManager(enemyManager) {
	m_defenders.reserve(128);
}

void DefenderManager::clear() {
	for (auto& defender : m_defenders) {
		m_collisionSystem.destroyCollider(defender->colliderHandle);
	}
	m_defenders.clear();
	for (auto& row : m_defenderGrid) {
		for (auto& defender : row) {
			defender = nullptr;
		}
	}
	m_highlightShader = m_resourceSystem.getShader("highlight");
}

void DefenderManager::clear(int row, int column) {
	if (row >= 0 && row < ROWS && column >= 0 && column < COLS) {
		auto& defender = m_defenderGrid[row][column];
		if (defender) {
			if (m_highlightedDefender == defender) {
				unhighlight();
			}
			m_collisionSystem.destroyCollider(defender->colliderHandle);
			m_onDefenderDestroyedCallbacks.executeCallbacks(*defender);
			std::erase_if(m_defenders, [&defender](const std::unique_ptr<Defender>& d) { return d.get() == defender; });
			defender = nullptr;
		}
	}
}

void DefenderManager::draw(Atlas& atlas) {
	for (auto& defender : m_defenders) {
		DrawEllipse(defender->position.x + CELL_SIZE * 0.5f, defender->position.y + CELL_SIZE - 1, 12, 4, Fade(BLACK, 0.1f));
		if (defender.get() == m_highlightedDefender) {
			BeginShaderMode(*m_highlightShader);
			atlas.drawSprite(defender->animation.getSpriteInfo(), defender->position, defender->animation.getCurrentFrame(), Flip::None, defender->tint);
			EndShaderMode();
		} else {
			atlas.drawSprite(defender->animation.getSpriteInfo(), defender->position, defender->animation.getCurrentFrame(), Flip::None, defender->tint);
		}
	}
}

void DefenderManager::setState(Defender& defender, DefenderState state) {
	if (defender.state != state) {
		defender.task.clear();
		defender.state = state;
		defender.tint = WHITE;
		switch (state) {
		case DefenderState::On : defender.animation = Animation::createAnimation(defender.info->spriteEnabled); break;
		case DefenderState::Off: {
			defender.animation = Animation::createAnimation(defender.info->spriteDisabled);
			defender.tint = {100, 100, 100, 255};
			break;
		}
		case DefenderState::Shooting: {
			defender.animation = Animation::createAnimation(defender.info->spriteShoot);
			defender.task.addTask({[&defender](float dt) {
				if (defender.animation.isOver()) {
					defender.animation = Animation::createAnimation(defender.info->spriteEnabled);
					return true;
				}
				return false;
			}});
			defender.task.addTask({[&](float dt) {
				defender.prepareShootTime -= dt;
				if (defender.prepareShootTime <= 0) {
					m_updateResult.actions.push_back(BulletSpawnAction{.bulletType = *defender.info->bulletType, .position = defender.position});
					return true;
				}
				return false;
			}});
			break;
		}
		case DefenderState::Dying: {
			m_collisionSystem.destroyCollider(defender.colliderHandle);
			defender.animation = Animation::createAnimation(defender.info->spriteDying);
			m_musicManager.playSound("robot_death");
			break;
		}
		case DefenderState::Dead: break;
		}
	}
}

bool DefenderManager::hasDefender(int row, int column) const {
	return m_defenderGrid[row][column] != nullptr;
}

void DefenderManager::highlight(Defender& defender) {
	m_highlightedDefender = &defender;
}

void DefenderManager::unhighlight() {
	m_highlightedDefender = nullptr;
}

Defender* DefenderManager::getHighlightedDefender() {
	return m_highlightedDefender;
}

DefenderUpdateResult DefenderManager::update(float dt) {
	m_updateResult = {};
	for (auto it = m_defenders.begin(); it != m_defenders.end();) {
		auto& defender = *it;

		if (defender->state != DefenderState::Dying && defender->state != DefenderState::Dead) {
			m_collisionSystem.updateCollider(defender->colliderHandle, {defender->position.x, defender->position.y, 32, 32});
		}

		defender->animation.update(dt);
		if (defender->state != DefenderState::Off) {
			m_updateResult.amountOfBatteryDrain += dt * defender->info->batteryDrain;

			if (defender->info->bulletType) {
				switch (defender->state) {
				case DefenderState::On:
					if (m_enemyManager.findClosestEnemy({0, 0}, true) != nullptr || defender->info->shootWithoutEnemies) {
						defender->shootTime -= dt;
						if (defender->shootTime <= 0) {
							defender->shootTime = defender->info->shootCooldown;
							defender->prepareShootTime = defender->info->shootingAnimationTime;
							setState(*defender, DefenderState::Shooting);
						}
					}
					break;
				case DefenderState::Shooting:
					defender->task.update(dt);
					if (defender->task.isDone()) {
						defender->task.clear();
						setState(*defender, DefenderState::On);
					}
					break;
				}
			}
		}

		if (defender->hp <= 0 && defender->state != DefenderState::Dying && defender->state != DefenderState::Dead) {
			setState(*defender, DefenderState::Dying);
		}

		switch (defender->state) {
		case DefenderState::Dying:
			performDying(*defender);
			++it;
			break;
		case DefenderState::Dead:
			m_defenderGrid[defender->row][defender->column] = nullptr;
			m_onDefenderDestroyedCallbacks.executeCallbacks(*defender);
			if (m_highlightedDefender == defender.get()) {
				m_highlightedDefender = nullptr;
			}
			it = m_defenders.erase(it);
			break;
		default: ++it;
		}
	}
	return m_updateResult;
}

const std::vector<std::unique_ptr<Defender>>& DefenderManager::getDefenders() const {
	return m_defenders;
}

Defender& DefenderManager::spawnDefender(const DefenderTypeInfo* defenderTypeInfo, int row, int column) {
	auto defender = std::make_unique<Defender>();
	defender->info = defenderTypeInfo;
	defender->shootTime = defenderTypeInfo->firstShootCooldown;
	defender->position = Vector2{GRID_OFFSET.x + (float(column) * CELL_SIZE), GRID_OFFSET.y + (float(row) * CELL_SIZE) - 5};
	defender->column = column;
	defender->row = row;
	defender->animation = Animation::createAnimation(defenderTypeInfo->spriteEnabled);
	defender->hp = defender->info->maxHP;
	defender->colliderHandle = m_collisionSystem.createCollider(Collider::Flag::Defender, defender.get());
	defender->prepareShootTime = defender->info->shootingAnimationTime;
	m_defenderGrid[row][column] = defender.get();
	auto* defenderPtr = defender.get();
	m_defenders.push_back(std::move(defender));
	return *defenderPtr;
}

void DefenderManager::toggleDefender(int row, int column) {
	auto* defender = m_defenderGrid[row][column];
	if (!defender) {
		return;
	}

	if (defender->state == DefenderState::Off) {
		enableDefender(*defender);
	} else if (defender->state != DefenderState::Off) {
		disableDefender(*defender);
	}
}

Defender* DefenderManager::getDefender(int row, int column) {
	return m_defenderGrid[row][column];
}

void DefenderManager::enableDefender(Defender& defender) {
	if (defender.state != DefenderState::Dying && defender.state != DefenderState::Dead) {
		if (defender.state == DefenderState::Off) {
			setState(defender, DefenderState::On);
			m_musicManager.playSound("switch");
		}
	}
}

void DefenderManager::disableDefender(Defender& defender) {
	if (defender.state != DefenderState::Dying && defender.state != DefenderState::Dead) {
		if (defender.state != DefenderState::Off) {
			setState(defender, DefenderState::Off);
			m_musicManager.playSound("switch");
		}
	}
}

CallbackHandle DefenderManager::onDefenderDestroyed(std::function<void(Defender&)> callback) {
	return m_onDefenderDestroyedCallbacks.registerCallback(std::move(callback));
}

void DefenderManager::performDying(Defender& defender) {
	if (defender.animation.isOver()) {
		setState(defender, DefenderState::Dead);
	}
}
