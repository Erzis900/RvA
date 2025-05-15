#include "PortalManager.h"

#include "collisions/CollisionSystem.h"
#include "constants.h"

#include <iostream>

PortalManager::PortalManager(CollisionSystem& collisionSystem) : m_collisionSystem(collisionSystem) {}

void PortalManager::spawnPortals(const PortalTypeInfo* entranceInfo, const PortalTypeInfo* exitInfo, int inRow, int inCol, int outRow, int outCol) {
	auto entrance = createPortal(entranceInfo, inRow, inCol, WHITE);
	auto exit = createPortal(exitInfo, outRow, outCol, RED);

	m_portalPairs.push_back({std::move(entrance), std::move(exit)});
}

Portal* PortalManager::getExit(int entranceId) {
	for (auto& pair : m_portalPairs) {
		if (pair.entrance->id == entranceId) {
			return pair.exit.get();
		}
	}

	return nullptr;
}

std::unique_ptr<Portal> PortalManager::createPortal(const PortalTypeInfo* info, int row, int col, Color tint) {
	auto portal = std::make_unique<Portal>();

	portal->info = info;
	portal->type = info->type;
	portal->position = Vector2{GRID_OFFSET.x + (float(col) * CELL_SIZE), GRID_OFFSET.y + (float(row) * CELL_SIZE) - 5};
	portal->animation = Animation::createAnimation(info->spriteOpen);

	portal->lifespan = info->lifespan;
	portal->row = row;
	portal->column = col;
	portal->state = PortalState::Summoning;
	portal->tint = tint;
	portal->id = m_portalId++;
	portal->colliderHandle = m_collisionSystem.createCollider(Collider::Flag::Portal, portal.get());

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
	auto updatePortal = [dt, this](std::unique_ptr<Portal>& portal) -> bool {
		portal->animation.update(dt);

		switch (portal->state) {
		case PortalState::Summoning: performSummoning(portal); break;
		case PortalState::Idle	   : performIdle(portal, dt); break;
		case PortalState::Closing  : performClosing(portal, dt); break;
		case PortalState::Destroy  : return true;
		}

		return false;
	};

	for (auto itr = m_portalPairs.begin(); itr != m_portalPairs.end();) {
		bool shouldDestroyEntrance = updatePortal(itr->entrance);
		bool shouldDestroyExit = updatePortal(itr->exit);

		if (shouldDestroyEntrance || shouldDestroyExit) {
			itr = m_portalPairs.erase(itr);
		} else {
			++itr;
		}
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

void PortalManager::performIdle(std::unique_ptr<Portal>& portal, float dt) {
	m_collisionSystem.updateCollider(portal->colliderHandle, {portal->position.x, portal->position.y, CELL_SIZE, CELL_SIZE});

	portal->lifespan -= dt;
	if (portal->lifespan <= 0.f) {
		setState(portal, PortalState::Closing);

		m_collisionSystem.destroyCollider(portal->colliderHandle);
		portal->animation = Animation::createAnimation(portal->info->spriteClose);
	}
}

void PortalManager::performClosing(std::unique_ptr<Portal>& portal, float dt) {
	if (portal->animation.isOver()) {
		setState(portal, PortalState::Destroy);
	}
}

void PortalManager::clear() {
	for (auto& pair : m_portalPairs) {
		m_collisionSystem.destroyCollider(pair.entrance->colliderHandle);
		m_collisionSystem.destroyCollider(pair.exit->colliderHandle);
	}

	m_portalPairs.clear();
}
