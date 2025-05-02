#include "Game.h"
#include "Session.h"
#include <raymath.h>

Session::Session(Game& game, CollisionSystem& collisionSystem)
	: m_game(game)
	, m_collisionSystem(collisionSystem)
	, m_defenderManager(game.getAtlas(), m_collisionSystem)
	, m_enemyManager(game, game.getEnemyTypeRegistry(), m_collisionSystem)
	, m_bulletManager(m_enemyManager, m_collisionSystem)
{
	m_enemyManager.onEnemiesDestroyed([this, &game](int numberOfDestroyedEnemies) {
		m_numberOfDestroyedEnemies += numberOfDestroyedEnemies;
	});

	m_collisionSystem.addColliderMatch(Collider::Flag::Bullet, Collider::Flag::Enemy);
	m_collisionSystem.addColliderMatch(Collider::Flag::Defender, Collider::Flag::Enemy);
	m_collisionSystem.addColliderMatch(Collider::Flag::BaseWall, Collider::Flag::Enemy);
	m_collisionSystem.onCollision([this](const Collision& collision) { manageCollision(collision); });
	m_baseWall.colliderHandle = m_collisionSystem.createCollider(Collider::Flag::BaseWall, &m_baseWall);
	m_collisionSystem.updateCollider(
		m_baseWall.colliderHandle,
		{ CELL_SIZE - 5, CELL_SIZE, 5, CELL_SIZE * ROWS }
	);
}

Session::~Session()
{
	m_collisionSystem.destroyCollider(m_baseWall.colliderHandle);
}

void Session::drawGrid()
{
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			int x = CELL_SIZE + col * CELL_SIZE;
			int y = CELL_SIZE + row * CELL_SIZE;

			DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, BLACK);
		}
	}
}

void Session::update(Game& game, float dt)
{
	m_enemyManager.update(dt);
	m_collisionSystem.update(dt);

	performDefenderSpawnOnInput();
	auto result = m_defenderManager.update(dt);
	performActions(result.actions);
	updateBatteryAndScraps(result.amountOfScrapsGain, result.amountOfBatteryDrain);

	m_bulletManager.update(dt);
}

void Session::draw(Game& game)
{
	drawGrid();

	m_defenderManager.draw();
	m_enemyManager.draw();
	m_bulletManager.draw();
	m_collisionSystem.draw();
}

int Session::getNumberOfDestroyedEnemies() const
{
	return m_numberOfDestroyedEnemies;
}

float Session::getBatteryCharge() const
{
	return m_batteryCharge;
}

float Session::getScraps() const
{
	return m_scraps;
}

void Session::setSelectedDefender(std::optional<DefenderType> type)
{
	m_selectedDefender = type;
}

const DefenderManager& Session::getDefenderManager() const
{
	return m_defenderManager;
}

const EnemyManager& Session::getEnemyManager() const
{
	return m_enemyManager;
}

void Session::updateBatteryAndScraps(float scrapGain, float batteryDrain)
{
	m_scraps += scrapGain;
	m_batteryCharge -= batteryDrain;
	m_batteryCharge = Clamp(m_batteryCharge, 0, 100);
}

void Session::performDefenderSpawnOnInput()
{
	if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)))
	{
		auto mousePos = GetMousePosition();
		int row = int(mousePos.y) / CELL_SIZE - 1;
		int column = int(mousePos.x) / CELL_SIZE - 1;

		if (row >= 0 && row < ROWS && column >= 0 && column < COLS)
		{
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && m_selectedDefender)
			{
				if (canPlaceDefender(row, column))
				{
					auto defenderInfo = m_game.getDefenderRegistry().getDefenderInfo(*m_selectedDefender);
					if (defenderInfo && canAffordCost(defenderInfo->cost))
					{
						m_defenderManager.spawnDefender(defenderInfo, row, column);
						m_scraps -= defenderInfo->cost;
					}
				}
			}
			else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
			{
				if (!canPlaceDefender(row, column))
				{
					m_defenderManager.toggleDefender(row, column);
				}
			}
		}
	}
}

void Session::performActions(const Actions& actions)
{
	for (auto& action : actions)
	{
		std::visit([this](auto&& action) { performAction(action); }, action);
	}
}

void Session::performAction(const BulletSpawnAction& action)
{
	auto bulletInfo = m_game.getBulletTypeRegistry().getBulletInfo(action.bulletType);
	if (bulletInfo)
	{
		m_bulletManager.spawnBullet(*bulletInfo, action.position);
	}
}

bool Session::canAffordCost(int cost) const
{
	return cost <= m_scraps;
}

bool Session::canPlaceDefender(int x, int y) const
{
	return !m_defenderManager.hasDefender(x, y);
}

void Session::manageCollision(const Collision& collision)
{
	switch (collision.mask)
	{
	case cMask(Collider::Flag::Bullet, Collider::Flag::Enemy):
		manageBulletEnemyCollision(collision);
		break;
	case cMask(Collider::Flag::Defender, Collider::Flag::Enemy):
		manageDefenderEnemyCollision(collision);
		break;
	case cMask(Collider::Flag::BaseWall, Collider::Flag::Enemy):
		manageBaseWallEnemyCollision(collision);
		break;
	}
}

void Session::manageBulletEnemyCollision(const Collision& collision)
{
	if (collision.event == CollisionEvent::Enter || collision.event == CollisionEvent::Ongoing)
	{
		const auto& [bullet, enemy] = collision.extractOwners<Bullet, Enemy>();
		m_bulletManager.executeHit(*bullet, *enemy);
	}
}

void Session::manageDefenderEnemyCollision(const Collision& collision)
{
	const auto& [defender, enemy] = collision.extractOwners<Defender, Enemy>();
	if (enemy->isDying())
	{
		return;
	}

	switch (collision.event)
	{
	case CollisionEvent::Enter:
		enemy->setState(EnemyState::PrepareToAttack);
		break;
	case CollisionEvent::Exit:
		enemy->setState(EnemyState::Moving);
		break;
	case CollisionEvent::Ongoing:
		if (enemy->getState() == EnemyState::ReadyToAttack)
		{
			defender->hp -= static_cast<int>(enemy->getInfo()->defenderDamage);
			enemy->setState(EnemyState::PrepareToAttack);
		}
		else if (enemy->getState() != EnemyState::PrepareToAttack)
		{
			enemy->setState(EnemyState::PrepareToAttack);
		}
		break;
	}
}

void Session::manageBaseWallEnemyCollision(const Collision& collision)
{
	const auto& [defender, enemy] = collision.extractOwners<Defender, Enemy>();
	if (enemy->isDying())
	{
		return;
	}

	switch (collision.event)
	{
	case CollisionEvent::Enter:
		enemy->setState(EnemyState::PrepareToAttack);
		break;
	case CollisionEvent::Exit:
		break;
	case CollisionEvent::Ongoing:
		if (enemy->getState() == EnemyState::ReadyToAttack)
		{
			m_batteryCharge -= static_cast<int>(enemy->getInfo()->baseWallDamage);
			enemy->applyDamage({ 50, false, DamageSource::BaseWall });
			enemy->setState(EnemyState::PrepareToAttack);
		}
		else if (enemy->getState() != EnemyState::PrepareToAttack)
		{
			enemy->setState(EnemyState::PrepareToAttack);
		}
		break;
	}
}