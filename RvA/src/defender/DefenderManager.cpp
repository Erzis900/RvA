#include "DefenderManager.h"

#include "collisions/CollisionSystem.h"

DefenderManager::DefenderManager(CollisionSystem& collisionSystem) : m_collisionSystem(collisionSystem) {
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
		atlas.drawSprite(defender->animation.getSpriteInfo(), defender->position, defender->animation.getCurrentFrame(), Flip::None, defender->tint);
	}
}

DefenderUpdateResult DefenderManager::update(float dt) {
	DefenderUpdateResult result;
	for (auto it = m_defenders.begin(); it != m_defenders.end();) {
		auto& defender = *it;

		m_collisionSystem.updateCollider(defender->colliderHandle, {defender->position.x, defender->position.y, 32, 32});

		defender->animation.update(dt);
		if (defender->state != DefenderState::Off) {
			result.amountOfBatteryDrain += dt * defender->info->batteryDrain;

			if (defender->info->scrapsGain != 0) {
				defender->scrapsGainTime += dt;
				if (defender->scrapsGainTime > 1.f) {
					defender->scrapsGainTime = 0;
					result.amountOfScrapsGain += defender->info->scrapsGain;
				}
			}

			if (defender->info->bulletType) {
				switch (defender->state) {
				case DefenderState::On:
					defender->shootTime -= dt;
					if (defender->shootTime <= 0) {
						defender->shootTime = defender->info->firstShootCooldown;
						setState(*defender, DefenderState::PrepareToShoot);
					}
					break;
				case DefenderState::PrepareToShoot: performPrepareShoot(*defender, dt); break;
				case DefenderState::ReadyToShoot:
					defender->prepareShootTime = defender->info->shootingAnimationTime;
					result.actions.push_back(BulletSpawnAction{.bulletType = *defender->info->bulletType, .position = defender->position});

					setState(*defender, DefenderState::On);
					break;
				}
			}
		}

		if (defender->hp <= 0) {
			m_defenderGrid[defender->row][defender->column] = nullptr;
			m_collisionSystem.destroyCollider((*it)->colliderHandle);
			m_onDefenderDestroyedCallbacks.executeCallbacks(defender->row, defender->column);
			it = m_defenders.erase(it);
		} else {
			++it;
		}
	}
	return result;
}

const std::vector<std::unique_ptr<Defender>>& DefenderManager::getDefenders() const {
	return m_defenders;
}

void DefenderManager::spawnDefender(const DefenderTypeInfo* defenderTypeInfo, int row, int column) {
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
	m_defenders.push_back(std::move(defender));
}

void DefenderManager::toggleDefender(int row, int column) {
	auto defender = m_defenderGrid[row][column];
	if (defender) {
		setState(*defender, defender->state == DefenderState::On ? DefenderState::Off : DefenderState::On);
	}
}

bool DefenderManager::hasDefender(int row, int column) const {
	auto defender = m_defenderGrid[row][column];
	return defender != nullptr;
}

void DefenderManager::setState(Defender& defender, DefenderState state) {
	if (defender.state != state) {
		defender.state = state;
		switch (state) {
		case DefenderState::On			  : defender.animation = Animation::createAnimation(defender.info->spriteEnabled); break;
		case DefenderState::Off			  : defender.animation = Animation::createAnimation(defender.info->spriteDisabled); break;
		case DefenderState::PrepareToShoot: defender.animation = Animation::createAnimation(defender.info->spriteShoot); break;
		default							  : break;
		}
	}
}

CallbackHandle DefenderManager::onDefenderDestroyed(std::function<void(int, int)> callback) {
	return m_onDefenderDestroyedCallbacks.registerCallback(std::move(callback));
}

void DefenderManager::performPrepareShoot(Defender& defender, float dt) {
	defender.prepareShootTime -= dt;
	if (defender.prepareShootTime <= 0.f) {
		setState(defender, DefenderState::ReadyToShoot);
	}
}
