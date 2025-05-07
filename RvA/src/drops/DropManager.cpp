#include "DropManager.h"

DropManager::DropManager(GameRegistry& gameRegistry, CollisionSystem& collisionSystem) {}

void DropManager::spawnDrop(const Vector2& position) {
	auto drop = std::make_unique<Drop>();
	drop->position = position;
	drop->enterAnimation = 0.f;
	m_drops.push_back(std::move(drop));
}

void DropManager::update(float dt) {
	for (auto& drop : m_drops) {
		drop->enterAnimation += dt;
		if (drop->enterAnimation >= 1.f) {
			drop->enterAnimation = 1.f;
		}
	}
}
