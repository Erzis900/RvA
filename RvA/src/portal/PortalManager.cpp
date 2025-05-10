#include "PortalManager.h"

#include "constants.h"

PortalManager::PortalManager(CollisionSystem& collisionSystem) : m_collisionSystem(collisionSystem) {}

void PortalManager::draw(Atlas& atlas) {}

void PortalManager::update(float dt) {
	for (auto it = m_portalPairs.begin(); it != m_portalPairs.end();) {
		auto& entrance = it->entrance;
		auto& exit = it->exit;
	}
}

void PortalManager::spawnPortals(const PortalTypeInfo* portalTypeInfo, int inRow, int inCol, int outRow, int outCol) {
	auto entrance = createPortal(portalTypeInfo, PortalType::Entrance, inRow, inCol, portalTypeInfo->spriteIdle);
	auto exit = createPortal(portalTypeInfo, PortalType::Exit, outRow, outCol, portalTypeInfo->spriteClose);

	m_portalPairs.push_back({std::move(entrance), std::move(exit)});
}

std::unique_ptr<Portal> PortalManager::createPortal(const PortalTypeInfo* info, PortalType type, int row, int col, AnimationData animationData) {
	auto portal = std::make_unique<Portal>();

	portal->info = info;
	portal->type = type;
	portal->position = Vector2{GRID_OFFSET.x + (float(col) * CELL_SIZE), GRID_OFFSET.y + (float(row) * CELL_SIZE) - 5};
	portal->animation = Animation::createAnimation(animationData);
	portal->hp = info->maxHP;
	portal->row = row;
	portal->column = col;
	portal->state = PortalState::Summoning;
	portal->tint = WHITE;

	// TODO collisions
	// portal->colliderHandle = m_collisionSystem.createCollider(Collider::Flag::Portal, portal.get());

	return portal;
}

