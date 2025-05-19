#pragma once
#include "Animation.h"
#include "PortalTypeRegistry.h"
#include "PortalTypes.h"
#include "collisions/Collider.h"

#include <memory>
#include <raylib.h>
#include <utility>
#include <vector>

class CollisionSystem;
class MusicManager;

enum class PortalState {
	Summoning,
	Idle,
	Closing,
	Destroy
};

struct Portal {
	const PortalTypeInfo* info{};
	PortalType type{};
	Vector2 position{};
	Animation animation;
	// TODO think whether portals should have hp instead of lifespan (game design)
	// int hp{};
	float lifespan{};
	int row{};
	int column{};
	ColliderHandle colliderHandle{};
	PortalState state{PortalState::Summoning};
	Color tint{WHITE};
	int id{};
};

struct PortalPair {
	std::unique_ptr<Portal> entrance;
	std::unique_ptr<Portal> exit;
};

class PortalManager {
public:
	PortalManager(CollisionSystem& collisionSystem, MusicManager& musicManager);

	void draw(Atlas& atlas);
	void update(float dt);
	void clear();

	void spawnPortals(const PortalTypeInfo* entranceTypeInfo, const PortalTypeInfo* exitInfo, int inRow, int inCol, int outRow, int outCol);
	Portal* getExit(int entranceId);

private:
	void setState(std::unique_ptr<Portal>& portal, PortalState state);

	void performSummoning(std::unique_ptr<Portal>& portal);
	void performIdle(std::unique_ptr<Portal>& portal, float dt);
	void performClosing(std::unique_ptr<Portal>& portal, float dt);

	// Leaving tint parameter possibly for future
	std::unique_ptr<Portal> createPortal(const PortalTypeInfo* info, int row, int col, Color tint);

	std::vector<PortalPair> m_portalPairs;
	CollisionSystem& m_collisionSystem;
	MusicManager& m_musicManager;

	int m_portalId{0};
};
