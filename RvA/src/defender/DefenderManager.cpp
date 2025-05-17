#include "DefenderManager.h"

#include "MusicManager.h"
#include "collisions/CollisionSystem.h"

DefenderManager::DefenderManager(CollisionSystem& collisionSystem, MusicManager& musicManager) : m_collisionSystem(collisionSystem), m_musicManager(musicManager) {
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
}

void DefenderManager::draw(Atlas& atlas) {
	for (auto& defender : m_defenders) {
		DrawEllipse(defender->position.x + CELL_SIZE * 0.5f, defender->position.y + CELL_SIZE - 1, 12, 4, Fade(BLACK, 0.1f));
		atlas.drawSprite(defender->animation.getSpriteInfo(), defender->position, defender->animation.getCurrentFrame(), Flip::None, defender->tint);
	}
}

void DefenderManager::setState(Defender& defender, DefenderState state) {
	if (defender.state != state) {
		defender.task.clear();
		defender.state = state;
		switch (state) {
		case DefenderState::On		: defender.animation = Animation::createAnimation(defender.info->spriteEnabled); break;
		case DefenderState::Off		: defender.animation = Animation::createAnimation(defender.info->spriteDisabled); break;
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
			break;
		}
		case DefenderState::Dead: break;
		}
	}
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

			if (defender->info->scrapsGain != 0) {
				defender->scrapsGainTime += dt;
				if (defender->scrapsGainTime > 1.f) {
					defender->scrapsGainTime = 0;
					m_updateResult.amountOfScrapsGain += defender->info->scrapsGain;
				}
			}

			if (defender->info->bulletType) {
				switch (defender->state) {
				case DefenderState::On:
					defender->shootTime -= dt;
					if (defender->shootTime <= 0) {
						defender->shootTime = defender->info->shootCooldown;
						defender->prepareShootTime = defender->info->shootingAnimationTime;
						setState(*defender, DefenderState::Shooting);
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
	auto defender = m_defenderGrid[row][column];
	if (defender && defender->state != DefenderState::Dying && defender->state != DefenderState::Dead) {
		setState(*defender, defender->state != DefenderState::Off ? DefenderState::Off : DefenderState::On);
		m_musicManager.playSound("switch");
	}
}

Defender* DefenderManager::getDefender(int row, int column) {
	return m_defenderGrid[row][column];
}

bool DefenderManager::hasDefender(int row, int column) const {
	auto defender = m_defenderGrid[row][column];
	return defender != nullptr;
}

CallbackHandle DefenderManager::onDefenderDestroyed(std::function<void(Defender&)> callback) {
	return m_onDefenderDestroyedCallbacks.registerCallback(std::move(callback));
}

void DefenderManager::performDying(Defender& defender) {
	if (defender.animation.isOver()) {
		setState(defender, DefenderState::Dead);
	}
}
