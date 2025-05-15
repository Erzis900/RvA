#pragma once

#include "Animation.h"
#include "Damage.h"
#include "GameAction.h"
#include "collisions/CollisionSystem.h"
#include "enemy/EnemyTypeRegistry.h"
#include "utilities/Interpolation.h"

#include <raylib.h>
#include <string>

class Game;
class Atlas;
class Animation;

enum class EnemyState {
	Idle,
	Moving,
	PrepareToAttack,
	ReadyToAttack,
	Action,
	Dying,
	Dead
};

class Enemy {
public:
	Enemy(Vector2 position, const EnemyTypeInfo* typeInfo, int row);

	GameAction update(float dt);
	void draw(Atlas& atlas);

	void applyDamage(const Damage& damage);
	void setSparkEffect(float duration);

	void setPosition(Vector2 position);

	auto getHp() const {
		return m_hp;
	}

	auto getRow() const {
		return m_row;
	}

	const EnemyTypeInfo* getInfo() const;

	const Vector2& getPosition() const {
		return m_position;
	}

	Vector2 getCenteredPosition() const;
	Rectangle calculateBoundingBox() const;
	ColliderHandle getColliderHandle() const;
	void setColliderHandle(ColliderHandle handle);
	const Damage& getLatestDamageApplied() const;

	void setState(EnemyState state);

	auto getState() const {
		return m_state;
	}

	bool isDying() const;
	bool isAttacking() const;

private:
	void performIdle(float dt);
	GameAction performMove(float dt);
	void performPrepareAttack(float dt);
	void performDying();

	void setAnimation(const AnimationData& animationData);

	void setupBehavior(const EnemyBehaviorInfo& behaviorInfo);
	void setupBehavior(const PortalSpawnBehaviorInfo& behaviorInfo);
	void setupBehavior(const std::monostate& behaviorInfo);

	GameAction updateBehavior(EnemyBehavior& behaviorInfo, float dt);
	GameAction updateBehavior(PortalSpawnBehavior& behaviorInfo, float dt);
	GameAction updateBehavior(std::monostate& behaviorInfo, float dt);

	std::string m_spriteName;
	Vector2 m_position{};
	float m_hp{};
	float m_attackTime{};
	int m_row{};
	Interpolation<> m_damageTakenAnimation;
	Color m_tint{WHITE};
	Damage m_latestDamageApplied;
	float m_sparkEffectTimeRemain{0.f};

	Animation m_animation;
	Animation m_sparkEffect;
	EnemyState m_state;
	const EnemyTypeInfo* m_typeInfo{};
	ColliderHandle m_colliderHandle{};
	EnemyBehavior m_behavior{};
};
