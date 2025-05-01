#pragma once

class DefenderManager;
class EnemyManager;
class BulletManager;

// A simple class to track and debug collisions

class CollisionSystem
{
public:
	// TODO(Gerark) - This implementation is going to be quite specific and ad-hoc for the game
	CollisionSystem(const DefenderManager& defenderManager, const EnemyManager& enemyManager, const BulletManager& bulletManager);

	// If set to true the debug view shows all the boundings box for each collider
	void enableDebugView(bool enabled);
	void toggleDebugView();
	auto isDebugViewEnabled() const { return m_enableDebugView; }

	void draw();

private:
	bool m_enableDebugView{};

	const DefenderManager& m_defenderManager;
	const EnemyManager& m_enemyManager;
	const BulletManager& m_bulletManager;
};