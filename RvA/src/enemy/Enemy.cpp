#include "Enemy.h"

#include "portal/PortalManager.h"
#include "constants.h"

#include <iostream>
#include <raymath.h>

Enemy::Enemy(Vector2 position, const EnemyTypeInfo* typeInfo, int row) : m_position(position), m_row(row), m_typeInfo(typeInfo) {
	m_hp = typeInfo->maxHp;
	m_attackTime = typeInfo->attackTime;
	setAnimation(m_typeInfo->idleAnimation);
	setState(EnemyState::Idle);
}

void Enemy::setState(EnemyState state) {
	if (m_state != state) {
		m_state = state;

		switch (state) {
		case EnemyState::Idle  : setAnimation(m_typeInfo->idleAnimation); break;
		case EnemyState::Moving: setAnimation(m_typeInfo->moveAnimation); break;
		case EnemyState::PrepareToAttack:
		case EnemyState::ReadyToAttack:
			setAnimation(m_typeInfo->attackAnimation);
			m_attackTime = 0.5f;
			break;
		case EnemyState::Summoning: setAnimation(m_typeInfo->summonAnimation); break;

		case EnemyState::Dying	  : setAnimation(m_typeInfo->dyingAnimation); break;
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
			if (m_latestDamageApplied.bounceBackPower != 0) {
				m_position = Vector2Add(position, Vector2Lerp({0, 0}, {m_latestDamageApplied.bounceBackPower, 0}, value));
			}
		})
		.onComplete([this]() { m_tint = WHITE; });
}

void Enemy::setPosition(Vector2 position) {
	m_position = position;
}

std::optional<PortalSpawnAction> Enemy::update(float dt) {
	switch (m_state) {
	case EnemyState::Idle			: performIdle(dt); break;
	case EnemyState::Moving			: performMove(dt); break;
	case EnemyState::PrepareToAttack: performPrepareAttack(dt); break;
	case EnemyState::Dying			: performDying(); break;
	case EnemyState::Summoning:
		performSummoning();

		if (m_spawnedPortal) {										  // we spawn portal after animation is done (debatable but easier to do for me)
			std::tuple<int, int> coords = getCoordinates(m_position); // TODO: I believe it returns wrong col (+1)
			int col = std::get<1>(coords);

			return PortalSpawnAction{m_row, col - 2, m_row + 2, col - 4};
		}
		break;
	case EnemyState::Dead: break;
	}

	m_animation.update(dt);
	m_damageTakenAnimation.update(dt);

	return std::nullopt;
}

void Enemy::draw(Atlas& atlas) {
	DrawEllipse(m_position.x + CELL_SIZE * 0.5f, m_position.y + CELL_SIZE - 1, 12, 4, Fade(BLACK, 0.1f));
	atlas.drawSprite(m_animation.getSpriteInfo(), m_position, m_animation.getCurrentFrame(), Flip::None, m_tint);
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

void Enemy::performMove(float dt) {
	m_position.x -= m_typeInfo->speed * dt;

	if (m_typeInfo->type == EnemyType::Portal && !m_spawnedPortal) {
		if (m_position.x <= TEX_WIDTH - CELL_SIZE) {
			setState(EnemyState::Summoning);
		}
	}
}

void Enemy::performPrepareAttack(float dt) {
	m_attackTime -= dt;
	if (m_attackTime <= 0.f) {
		setState(EnemyState::ReadyToAttack);
	}
}

void Enemy::performSummoning() {
	if (m_animation.isOver()) {
		m_spawnedPortal = true;
		setState(EnemyState::Moving);
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
