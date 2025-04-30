#pragma once

#include <variant>

enum class BulletType
{
	SimpleBullet,
	Laser
};

/*
* Bullet Shot
*/
struct BulletShotData
{
	Vector2 velocity;
	float radius{};
	float damage{};
	float maxLifetime{};
};

/*
* Laser Beam
*/
struct LaserBeamData
{
	Vector2 startOffset{};
	float beamHeight{};
	float damage{};
	float auraSize{ 4 };
	Color beamColor{ 255, 0, 0, 255 };
	Color auraColor{ 255, 255, 255, 128 };
	float maxLifetime{};
	float shootAnimationSpeed{};
	float shootAnimationDuration{};
	float shootAnimationTime{};
	float beamWidth{};
};

using BulletData = std::variant<BulletShotData, LaserBeamData>;

struct Bullet2
{
	BulletData data;
	Vector2 position;
	Rectangle boundingBox;
	float lifetime{};
};