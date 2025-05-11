#pragma once

#include "Damage.h"
#include "collisions/CollisionSystem.h"

#include <raylib.h>
#include <variant>

/*
 * Bullet Shot
 */
struct BulletShotData {
	Vector2 velocity{};
	Vector2 offsetPos{};
	float radius{};
	DamageInfo damage{};
	float maxLifetime{};
	float time{};
};

/*
 * Laser Beam
 */
struct LaserBeamData {
	Vector2 startOffset{};
	float beamHeight{};
	DamageInfo damage{};
	float auraSize{4};
	Color beamStartColor{255, 0, 0, 255};
	Color beamEndColor{255, 255, 255, 128};
	float maxLifetime{};
	float shootAnimationSpeed{};
	float shootAnimationDuration{};
	float shootAnimationTime{};
	float beamWidth{};
};

/*
 * Chasing Shot
 */
struct ChasingShotData {
	Vector2 startOffset{};
	float radius{};
	DamageInfo damage{};
	float maxLifetime{};
	float speed{};
	Color color{};

	Vector2 targetPosition{};
	Vector2 direction{};
};

using BulletData = std::variant<BulletShotData, ChasingShotData, LaserBeamData>;

struct Bullet {
	BulletData data{};
	Vector2 position{};
	ColliderHandle colliderHandle{};
	float lifetime{};
};
