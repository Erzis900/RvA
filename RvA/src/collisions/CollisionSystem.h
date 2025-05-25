#pragma once

#include "Collider.h"
#include "FixedItemPool.h"
#include "utilities/TimedAction.h"

#include <functional>
#include <raylib.h>

class DefenderManager;
class EnemyManager;
class BulletManager;

/*
 * A very basic collision system
 */
class CollisionSystem {
public:
	// TODO(Gerark) - This implementation is going to be quite specific and ad-hoc for the game
	CollisionSystem();

	void addColliderMatch(Collider::Flag flagA, Collider::Flag flagB);

	// If set to true the debug view shows all the boundings box for each collider
	void enableDebugView(bool enabled);
	void toggleDebugView();

	auto isDebugViewEnabled() const {
		return m_enableDebugView;
	}

	void clearColliders();
	void update(float dt);
	void draw();

	ColliderHandle createCollider(Collider::Flag flag, ColliderOwner colliderOwner);
	void updateCollider(ColliderHandle handle, const Rectangle& boundingBox);
	void disableCollider(ColliderHandle handle);
	void enableCollider(ColliderHandle handle, Collider::Flag flag);
	void updateColliderPosition(ColliderHandle handle, const Vector2& position);
	void destroyCollider(ColliderHandle handle);

	void onCollision(std::function<void(const Collision&)> callback);

private:
	void manageScheduledDestruction();
	void manageCollisions();

	bool m_enableDebugView{};
	FixedItemPool<Collider, 256> m_colliders;
	std::vector<Collision> m_collisions;
	std::function<void(const Collision&)> m_collisionCallback;
	std::vector<int> m_collisionMatchers;
	std::vector<ColliderHandle> m_scheduledDestruction;
	TimedActionList m_debugDrawingList;
};
