#include "Enemy.h"

#include "constants.h"
#include "portal/PortalManager.h"

#include <iostream>
#include <raymath.h>

Enemy::Enemy(Vector2 position, const EnemyTypeInfo* typeInfo, int row) : m_position(position), m_row(row), m_typeInfo(typeInfo) {
	m_hp = typeInfo->maxHp;
	m_attackTime = typeInfo->attackTime;
	setAnimation(m_typeInfo->idleAnimation);

	m_sparkEffect = Animation::createAnimation(m_typeInfo->sparkEffect);
	setupBehavior(m_typeInfo->behavior);
	setState(EnemyState::Idle);
}

void Enemy::setState(EnemyState state) {
	if (m_state != state) {
		m_state = state;

		switch (state) {
		case EnemyState::Idle			: setAnimation(m_typeInfo->idleAnimation); break;
		case EnemyState::Moving			: setAnimation(m_typeInfo->moveAnimation); break;
		case EnemyState::PrepareToAttack:
		case EnemyState::ReadyToAttack	: {
			setAnimation(m_typeInfo->attackAnimation);
			m_attackTime = 0.5f;
			break;
		}
		case EnemyState::Dying: {
			setAnimation(m_typeInfo->dyingAnimation);
			break;
		}
		}
	}
}

bool Enemy::isDying() const {
	return getState() == EnemyState::Dying || getState() == EnemyState::Dead;
}

bool Enemy::isAttacking() const {
	return getState() == EnemyState::ReadyToAttack || getState() == EnemyState::PrepareToAttack;
}

void Enemy::applyDamage(const Damage& damage) {
	m_latestDamageApplied = damage;
	m_hp -= m_latestDamageApplied.value;
	m_damageTakenAnimation.start(0, 1, 0.25f)
		.onTick([this, position = m_position](const auto& value) {
			m_tint = colorLerp(RED, WHITE, value);
			if (m_hp > 0 && m_latestDamageApplied.bounceBackPower != 0) {
				m_position = Vector2Add(position, Vector2Lerp({0, 0}, {m_latestDamageApplied.bounceBackPower, 0}, value));
			}
		})
		.onComplete([this]() { m_tint = WHITE; });
}

void Enemy::setSparkEffect(float duration) {
	if (m_sparkEffectTimeRemain <= 0 && m_sparkEffect.isOver()) {
		m_sparkEffectTimeRemain = duration;
		m_sparkEffect.restart();
	}
}

void Enemy::setPosition(Vector2 position) {
	m_position = position;
}

GameAction Enemy::update(float dt) {
	GameAction action{};

	switch (m_state) {
	case EnemyState::Idle			: performIdle(dt); break;
	case EnemyState::Moving			: action = performMove(dt); break;
	case EnemyState::PrepareToAttack: performPrepareAttack(dt); break;
	case EnemyState::Dying			: performDying(); break;
	case EnemyState::Action			: action = updateBehavior(m_behavior, dt); break;
	case EnemyState::Dead			: break;
	}

	m_animation.update(dt);
	m_damageTakenAnimation.update(dt);

	m_sparkEffectTimeRemain -= dt;
	m_sparkEffect.update(dt);

	return action;
}

void Enemy::draw(Atlas& atlas) {
	DrawEllipse(m_position.x + CELL_SIZE * 0.5f, m_position.y + CELL_SIZE - 1, 12, 4, Fade(BLACK, 0.1f));
	atlas.drawSprite(m_animation.getSpriteInfo(), m_position, m_animation.getCurrentFrame(), Flip::None, m_tint);
	if (m_sparkEffectTimeRemain > 0.f && !m_sparkEffect.isOver()) {
		atlas.drawSprite(m_sparkEffect.getSpriteInfo(), {m_position.x - 5, m_position.y - 25}, m_sparkEffect.getCurrentFrame(), Flip::None, WHITE, Random::range(25.f, 35.f));
		atlas.drawSprite(m_sparkEffect.getSpriteInfo(), {m_position.x - 10, m_position.y - 0}, m_sparkEffect.getCurrentFrame(), Flip::None, WHITE, Random::range(-5.f, 5.f));
		atlas.drawSprite(m_sparkEffect.getSpriteInfo(), {m_position.x - 15, m_position.y + 25}, m_sparkEffect.getCurrentFrame(), Flip::None, WHITE, Random::range(-25.f, -35.f));
	}
}

const EnemyTypeInfo* Enemy::getInfo() const {
	return m_typeInfo;
}

Vector2 Enemy::getCenteredPosition() const {
	return Vector2Add(m_position, {16, 16});
}

Rectangle Enemy::calculateBoundingBox() const {
	constexpr Vector2 bbPadding = {10, 5};
	return Rectangle{m_position.x + bbPadding.x, m_position.y + bbPadding.y, 32 - bbPadding.x, 32 - bbPadding.y};
}

ColliderHandle Enemy::getColliderHandle() const {
	return m_colliderHandle;
}

void Enemy::setColliderHandle(ColliderHandle handle) {
	m_colliderHandle = handle;
}

const Damage& Enemy::getLatestDamageApplied() const {
	return m_latestDamageApplied;
}

void Enemy::performIdle(float dt) {
	// We immediately change state to Move for now
	setState(EnemyState::Moving);
}

GameAction Enemy::performMove(float dt) {
	m_position.x -= m_typeInfo->speed * dt;

	return updateBehavior(m_behavior, dt);
}

void Enemy::performPrepareAttack(float dt) {
	m_attackTime -= dt;
	if (m_attackTime <= 0.f) {
		setState(EnemyState::ReadyToAttack);
	}
}

void Enemy::performDying() {
	if (m_animation.isOver()) {
		setState(EnemyState::Dead);
	}
}

void Enemy::setAnimation(const AnimationData& animationData) {
	m_animation = Animation::createAnimation(animationData);
}

void Enemy::setupBehavior(const EnemyBehaviorInfo& behaviorInfo) {
	std::visit([&](auto&& behavior) { setupBehavior(behavior); }, behaviorInfo);
}

void Enemy::setupBehavior(const PortalSpawnBehaviorInfo& behaviorInfo) {
	m_behavior = PortalSpawnBehavior{
		.currentChance = behaviorInfo.baseChanceToSpawn,
		.timeChance = behaviorInfo.timeBeforeActing,
		.numberOfSpawnedPortals = 0,
	};
}

void Enemy::setupBehavior(const std::monostate& behaviorInfo) {}

GameAction Enemy::updateBehavior(EnemyBehavior& behavior, float dt) {
	return std::visit([&](auto&& arg) { return updateBehavior(arg, dt); }, behavior);
}

GameAction Enemy::updateBehavior(PortalSpawnBehavior& behavior, float dt) {
	auto& info = std::get<PortalSpawnBehaviorInfo>(m_typeInfo->behavior);

	if (m_state == EnemyState::Action) {
		if (m_animation.isOver()) {
			behavior.numberOfSpawnedPortals++;

			auto castRange = info.portalCastRange.generate();
			auto row = info.rowDistance.generate();
			auto col = info.columnDistance.generate();

			auto [currentRow, entranceCol] = getCoordinates(getCenteredPosition());
			entranceCol -= castRange;

			setState(EnemyState::Moving);

			return PortalSpawnAction{currentRow, clampColumn(entranceCol), clampRow(currentRow + row), clampColumn(entranceCol - col)};
		}
	} else if (behavior.numberOfSpawnedPortals <= 0) {
		behavior.timeChance -= dt;
		behavior.currentChance += info.chanceIncreaseOverTime * dt;

		if (behavior.timeChance <= 0) {
			if (Random::range(0.f, 100.f) < behavior.currentChance) {
				behavior.currentChance = 0.f;
				behavior.timeChance = info.timeBeforeActingAgain;
				setAnimation(info.animation);
				setState(EnemyState::Action);
			} else {
				behavior.timeChance = info.timeBeforeActingAgain;
			}
		}
	}

	return {};
}

GameAction Enemy::updateBehavior(std::monostate& behaviorInfo, float dt) {
	return {};
}
