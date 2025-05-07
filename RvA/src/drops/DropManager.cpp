#include "DropManager.h"

#include "utilities/Interpolation.h"

#include <raymath.h>

DropManager::DropManager(const GameRegistry& gameRegistry, CollisionSystem& collisionSystem) {
	m_collectedDrops.reserve(32);
}

void DropManager::spawnDrop(const DropTypeInfo* dropInfo, const Vector2& position, int amount) {
	auto drop = std::make_unique<Drop>();
	drop->position = position;
	drop->startPosition = position;
	drop->endPosition = {60, 0};
	drop->enterAnimation = 0.f;
	drop->info = dropInfo;
	drop->amount = amount;
	m_drops.push_back(std::move(drop));
}

void DropManager::clear() {
	m_drops.clear();
	m_collectedDrops.clear();
}

void DropManager::update(float dt) {
	m_collectedDrops.clear();

	static const float animationDuration = 1;
	for (auto it = m_drops.begin(); it != m_drops.end();) {
		auto& drop = *it;

		if (drop->enterAnimation >= animationDuration) {
			m_collectedDrops.emplace_back(drop->info, drop->amount);
			it = m_drops.erase(it);
		} else {
			drop->enterAnimation += dt;
			drop->enterAnimation = Clamp(drop->enterAnimation, 0, animationDuration);
			float t = drop->enterAnimation / animationDuration;
			t = getEasingFunction(Ease::InOutSine)(t);
			drop->position = GetSplinePointBezierCubic(drop->startPosition, Vector2Add(drop->startPosition, {0, 50}), Vector2Add(drop->endPosition, {0, 50}), drop->endPosition, t);

			++it;
		}
	}

	if (!m_collectedDrops.empty()) {
		m_onDropsCollectedCallbacks.executeCallbacks(m_collectedDrops);
	}
}

void DropManager::draw(Atlas& atlas) {
	for (auto& drop : m_drops) {
		auto sprite = drop->info->idleAnimation.getSpriteInfo();
		atlas.drawSprite(sprite, drop->position, drop->info->idleAnimation.getCurrentFrame(), Flip::None, WHITE);
	}
}
