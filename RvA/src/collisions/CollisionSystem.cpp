#include "collisions/CollisionSystem.h"

CollisionSystem::CollisionSystem()
{
	m_collisions.reserve(128);
}

void CollisionSystem::addColliderMatch(Collider::Flag flagA, Collider::Flag flagB)
{
	m_collisionMatchers.push_back(flagA | flagB);
}

void CollisionSystem::enableDebugView(bool enabled)
{
	m_enableDebugView = enabled;
}

void CollisionSystem::toggleDebugView()
{
	m_enableDebugView = !m_enableDebugView;
}

void CollisionSystem::update(float dt)
{
	m_collisions.clear();

	manageScheduledDestruction();
	manageCollisions();

	for (auto& collision : m_collisions)
	{
		m_collisionCallback(collision);
	}

	if (m_enableDebugView)
	{
		for (Collision& collision : m_collisions)
		{
			if (collision.event == CollisionEvent::Enter)
			{
				auto bbA = collision.colliderA->boundingBox;
				auto bbB = collision.colliderB->boundingBox;
				m_debugDrawingList.addTimedAction([bbA, bbB]() {
					DrawRectangleLinesEx(bbA, 1, RED);
					DrawRectangleLinesEx(bbB, 1, RED);
					}, 0.5f);
			}
		}

		m_debugDrawingList.updateTimers(dt);
	}
}

void CollisionSystem::manageScheduledDestruction()
{
	for (auto& handle : m_scheduledDestruction)
	{
		Collider* collider = m_colliders.getItem(handle);
		for (auto& otherColliderHandle : collider->collidingWith)
		{
			Collider* otherCollider = m_colliders.getItem(otherColliderHandle);
			otherCollider->collidingWith.erase(handle);
			m_collisions.push_back({
				otherCollider->flag | collider->flag,
				CollisionEvent::Exit,
				collider,
				otherCollider
				});
		}

		m_colliders.releaseItem(handle);
	}
	m_scheduledDestruction.clear();
}

void CollisionSystem::manageCollisions()
{
	int i = 1;
	m_colliders.forEachActive([this, &i](ColliderHandle handleA, Collider& cA) {
		m_colliders.forEachActive([this, &i, &cA](ColliderHandle handleB, Collider& cB) {
			bool shouldCheck = false;
			int mask = cA.flag | cB.flag;
			for (auto& matcher : m_collisionMatchers) {
				shouldCheck |= (matcher == mask);
			}

			if (shouldCheck) {
				if (CheckCollisionRecs(cA.boundingBox, cB.boundingBox)) {
					auto firstCollision =
						cA.collidingWith.find(cB.handle) == cA.collidingWith.end() &&
						cB.collidingWith.find(cA.handle) == cB.collidingWith.end();

					auto event = CollisionEvent::Ongoing;
					if (firstCollision) {
						cA.collidingWith.insert(cB.handle);
						cB.collidingWith.insert(cA.handle);
						event = CollisionEvent::Enter;
					}
					m_collisions.push_back(Collision{ mask, event, &cA, &cB });
				}
				else {
					auto wasColliding =
						cA.collidingWith.find(cB.handle) != cA.collidingWith.end() ||
						cB.collidingWith.find(cA.handle) != cB.collidingWith.end();
					if (wasColliding)
					{
						cA.collidingWith.erase(cB.handle);
						cB.collidingWith.erase(cA.handle);
						m_collisions.push_back(Collision{ mask, CollisionEvent::Exit, &cA, &cB });
					}
				}
			}
		}, i);
		++i;
	});
}

void CollisionSystem::draw()
{
	if(m_enableDebugView)
	{
		m_colliders.forEachActive([](auto handle, auto& collider) {
			DrawRectangleLinesEx(collider.boundingBox, 1, YELLOW);
		});
		m_debugDrawingList.callActions();
	}
}

ColliderHandle CollisionSystem::createCollider(Collider::Flag flag, ColliderOwner colliderOwner)
{
	auto colliderHandle = m_colliders.createItem({ 0, flag, {}, std::move(colliderOwner)});
	auto* collider = m_colliders.getItem(colliderHandle);
	collider->handle = colliderHandle;
	return colliderHandle;
}

void CollisionSystem::updateCollider(ColliderHandle handle, const Rectangle& boundingBox)
{
	auto collider = m_colliders.getItem(handle);
	collider->boundingBox = boundingBox;
}

void CollisionSystem::updateColliderPosition(ColliderHandle handle, const Vector2& position)
{
	auto collider = m_colliders.getItem(handle);
	collider->boundingBox.x = position.x;
	collider->boundingBox.y = position.y;
}

void CollisionSystem::destroyCollider(ColliderHandle handle)
{
	m_colliders.getItem(handle);
	m_scheduledDestruction.push_back(handle);
}

void CollisionSystem::onCollision(std::function<void(const Collision&)> callback)
{
	m_collisionCallback = std::move(callback);
}

