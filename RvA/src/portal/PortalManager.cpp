#include "PortalManager.h"

#include "constants.h"

#include <iostream>

PortalManager::PortalManager(CollisionSystem& collisionSystem) : m_collisionSystem(collisionSystem) {}

void PortalManager::spawnPortals(const PortalTypeInfo* entranceInfo, const PortalTypeInfo* exitInfo, int inRow, int inCol, int outRow, int outCol) {
	auto entrance = createPortal(entranceInfo, inRow, inCol);
	auto exit = createPortal(exitInfo, outRow, outCol);

	m_portalPairs.push_back({std::move(entrance), std::move(exit)});
}

std::unique_ptr<Portal> PortalManager::createPortal(const PortalTypeInfo* info, int row, int col) {
	auto portal = std::make_unique<Portal>();

	portal->info = info;
	portal->type = info->type;
	portal->position = Vector2{GRID_OFFSET.x + (float(col) * CELL_SIZE), GRID_OFFSET.y + (float(row) * CELL_SIZE) - 5};
	portal->animation = Animation::createAnimation(info->spriteClose);

	portal->hp = info->maxHP;
	portal->row = row;
	portal->column = col;
	portal->state = PortalState::Summoning;
	portal->tint = WHITE;

	// TODO collisions
	// portal->colliderHandle = m_collisionSystem.createCollider(Collider::Flag::Portal, portal.get());

	return portal;
}

void PortalManager::draw(Atlas& atlas) {
	for (auto& pair : m_portalPairs) {
		auto& entrance = pair.entrance;
		auto& exit = pair.exit;

		atlas.drawSprite(entrance->animation.getSpriteInfo(), entrance->position, entrance->animation.getCurrentFrame(), Flip::None, entrance->tint);
		atlas.drawSprite(exit->animation.getSpriteInfo(), exit->position, exit->animation.getCurrentFrame(), Flip::None, exit->tint);
	}
}

void PortalManager::update(float dt) {
	auto updatePortal = [dt, this](std::unique_ptr<Portal>& portal) {
		portal->animation.update(dt);

		switch (portal->state) {
		case PortalState::Summoning: performSummoning(portal); break;
		}
	};

	for (auto& pair : m_portalPairs) {
		updatePortal(pair.entrance);
		updatePortal(pair.exit);
	}
}

void PortalManager::setState(std::unique_ptr<Portal>& portal, PortalState state) {
	portal->state = state;
}

void PortalManager::performSummoning(std::unique_ptr<Portal>& portal) {
	if (portal->animation.isOver()) {
		setState(portal, PortalState::Idle);
		portal->animation = Animation::createAnimation(portal->info->spriteIdle);
	}
}

void PortalManager::clear() {
	m_portalPairs.clear();
}
