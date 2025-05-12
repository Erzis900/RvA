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

enum class PortalState {
	Summoning,
	Idle
};

struct Portal {
	const PortalTypeInfo* info{};
	PortalType type{};
	Vector2 position{};
	Animation animation;
	int hp{};
	int row{};
	int column{};
	ColliderHandle colliderHandle{};
	PortalState state{PortalState::Summoning};
	Color tint{WHITE};
	int id;
};

struct PortalPair {
	std::unique_ptr<Portal> entrance;
	std::unique_ptr<Portal> exit;
};

class PortalManager {
public:
	PortalManager(CollisionSystem& collisionSystem);

	void draw(Atlas& atlas);
	void update(float dt);
	void clear();

	void spawnPortals(const PortalTypeInfo* entranceTypeInfo, const PortalTypeInfo* exitInfo, int inRow, int inCol, int outRow, int outCol);
	std::unique_ptr<Portal>& getExit(int entranceId);

private:
	void setState(std::unique_ptr<Portal>& portal, PortalState state);

	void performSummoning(std::unique_ptr<Portal>& portal);

	std::unique_ptr<Portal> createPortal(const PortalTypeInfo* info, int row, int col);

	std::vector<PortalPair> m_portalPairs;
	CollisionSystem& m_collisionSystem;

	int m_portalId{0};
};
