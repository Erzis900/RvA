#include "DropManager.h"

#include "utilities/Interpolation.h"

#include <raymath.h>

DropManager::DropManager(const GameRegistry& gameRegistry, CollisionSystem& collisionSystem) {
	m_collectedDrops.reserve(32);
}

void DropManager::spawnDrop(const DropTypeInfo* dropInfo, const Vector2& position, int amount) {
	auto drop = std::make_unique<Drop>();
	auto* spriteInfo = dropInfo->idleAnimation.getSpriteInfo();
	drop->size = dropInfo->size.value_or(Vector2{(float)spriteInfo->frames[0].width, (float)spriteInfo->frames[0].height});
	drop->position = Vector2Subtract(position, Vector2Scale(drop->size, 0.5f));
	drop->startPosition = drop->position;
	drop->enterAnimation = 0.f;
	drop->info = dropInfo;
	drop->amount = amount;
	m_collectedDrops.emplace_back(drop->info, drop->amount);
	m_onDropsCollectedCallbacks.executeCallbacks(m_collectedDrops);
	m_drops.push_back(std::move(drop));
}

void DropManager::clear() {
	m_drops.clear();
	m_collectedDrops.clear();
}

void DropManager::update(float dt) {
	m_collectedDrops.clear();

	for (auto it = m_drops.begin(); it != m_drops.end();) {
		auto& drop = *it;

		if (drop->enterAnimation >= drop->info->animationDuration) {
			it = m_drops.erase(it);
		} else {
			drop->enterAnimation += dt;
			drop->enterAnimation = Clamp(drop->enterAnimation, 0, drop->info->animationDuration);
			float t = drop->enterAnimation / drop->info->animationDuration;
			t = getEasingFunction(Ease::InSine)(t);
			drop->position = GetSplinePointLinear(drop->startPosition, Vector2Add(drop->startPosition, {0, -20}), t);

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
		atlas.drawSprite(sprite, Vector2Add(drop->position, {-5, -5}), drop->size, drop->info->idleAnimation.getCurrentFrame(), Flip::None, Fade(BLACK, 0.25));
		atlas.drawSprite(sprite, drop->position, drop->size, drop->info->idleAnimation.getCurrentFrame(), Flip::None, WHITE);
		::DrawTextEx(GetFontDefault(), TextFormat("x%d", drop->amount), Vector2Add(drop->position, {0, 0}), 10, 1, WHITE);
	}
}
