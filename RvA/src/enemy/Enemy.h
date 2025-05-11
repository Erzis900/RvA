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
	Summoning,
	Dying,
	Dead
};

class Enemy {
public:
	Enemy(Vector2 position, const EnemyTypeInfo* typeInfo, int row);

	std::optional<PortalSpawnAction> update(float dt);
	void draw(Atlas& atlas);

	void applyDamage(const Damage& damage);

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
	void performMove(float dt);
	void performPrepareAttack(float dt);
	void performDying();
	void performSummoning();

	void setAnimation(const AnimationData& animationData);

	std::string m_spriteName;
	Vector2 m_position{};
	float m_hp{};
	float m_attackTime{};
	int m_row{};
	Interpolation<> m_damageTakenAnimation;
	Color m_tint{WHITE};
	Damage m_latestDamageApplied;
	bool m_spawnedPortal{false};

	Animation m_animation;
	EnemyState m_state;
	const EnemyTypeInfo* m_typeInfo{};
	ColliderHandle m_colliderHandle{};
};
