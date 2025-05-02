#include "PlayState.h"
#include "WinState.h"
#include "LostState.h"
#include "MenuState.h"
#include <raylib.h>
#include <raymath.h>
#include "Game.h"
#include "constants.h"

PlayState::PlayState(Game& game)
	: m_game(game)
	, m_defenderManager(game.getAtlas(), game.getGUI(), m_collisionSystem)
	, m_enemyManager(game, game.getEnemyTypeRegistry(), m_collisionSystem)
	, m_bulletManager(m_enemyManager, m_collisionSystem)
	, m_hud(m_game.getAtlas(), m_game.getGUI())
{
	m_enemyManager.onEnemiesDestroyed([this, &game](int numberOfDestroyedEnemies) {
		m_numberOfDestroyedEnemies += numberOfDestroyedEnemies;
		if (m_numberOfDestroyedEnemies >= m_numberOfEnemiesToKill) {
			goToWinState(game);
		}
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

	setupHUD();
}

PlayState::~PlayState()
{
	m_collisionSystem.destroyCollider(m_baseWall.colliderHandle);
}

void PlayState::onEnter(Game& game)
{
	game.getMusicManager().play(game.getMusicManager().getGameMusic());
}

void PlayState::onExit(Game& game)
{
	game.getMusicManager().stop(game.getMusicManager().getGameMusic());
}

void PlayState::drawGrid()
{
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			int x = CELL_SIZE + col * CELL_SIZE;
			int y = CELL_SIZE + row * CELL_SIZE;

			DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, BLACK);
		}
	}
}

void PlayState::update(Game& game, float dt)
{
	if (m_isGamePaused) return;

	m_enemyManager.update(dt);
	m_collisionSystem.update(dt);

	performDefenderSpawnOnInput();
	auto result = m_defenderManager.update(dt);
	performActions(result.actions);
	updateBatteryAndScraps(result.amountOfScrapsGain, result.amountOfBatteryDrain);
	if (m_batteryCharge <= 0)
	{
		m_game.setState(std::make_unique<LostState>());
	}

	m_bulletManager.update(dt);

	if constexpr (DEV_MODE)
	{
		if (IsKeyPressed(KEY_W))
		{
			goToWinState(game);
		}
		else if (IsKeyPressed(KEY_F1))
		{
			m_collisionSystem.toggleDebugView();
		}
	}
}

void PlayState::draw(Game& game)
{
	drawGrid();

	m_defenderManager.draw();
	m_enemyManager.draw();
	m_bulletManager.draw();
	m_collisionSystem.draw();
	m_hud.draw();
}

void PlayState::goToWinState(Game& game)
{
	game.setState(std::make_unique<WinState>(game));
}

void PlayState::updateBatteryAndScraps(float scrapGain, float batteryDrain)
{
	m_scraps += scrapGain;
	m_batteryCharge -= batteryDrain;
	m_batteryCharge = Clamp(m_batteryCharge, 0, 100);

	m_hud.data().batteryCharge = m_batteryCharge;
	m_hud.data().scrapsAmount = static_cast<int>(m_scraps);
}

void PlayState::performDefenderSpawnOnInput()
{
	if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)))
	{
		auto mousePos = GetMousePosition();
		int row = int(mousePos.y) / CELL_SIZE - 1;
		int column = int(mousePos.x) / CELL_SIZE - 1;

		if (row >= 0 && row < ROWS && column >= 0 && column < COLS)
		{
			auto defenderType = m_hud.data().selectedDefender;
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && defenderType)
			{
				if (canPlaceDefender(row, column))
				{
					auto defenderInfo = m_game.getDefenderRegistry().getDefenderInfo(*defenderType);
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

void PlayState::performActions(const Actions& actions)
{
	for (auto& action : actions)
	{
		std::visit([this](auto&& action) { performAction(action); }, action);
	}
}

void PlayState::performAction(const BulletSpawnAction& action)
{
	auto bulletInfo = m_game.getBulletTypeRegistry().getBulletInfo(action.bulletType);
	if (bulletInfo)
	{
		m_bulletManager.spawnBullet(*bulletInfo, action.position);
	}
}

bool PlayState::canAffordCost(int cost) const
{
	return cost <= m_scraps;
}

bool PlayState::canPlaceDefender(int x, int y) const
{
	return !m_defenderManager.hasDefender(x, y);
}

void PlayState::setupHUD()
{
	auto& hudData = m_hud.data();
	for (const auto& [type, defenderInfo] : m_game.getDefenderRegistry().getDefenderInfos())
	{
		hudData.defenders.emplace_back(type, defenderInfo.spriteEnabled.spriteInfo, defenderInfo.cost);
	}

	m_hud.onMenuButtonPressed([this]() { m_game.setState(std::make_unique<MenuState>()); });
	m_hud.onPauseButtonPressed([this]() { togglePause(); });
	m_hud.onResumeButtonPressed([this]() { togglePause(); });
}

void PlayState::togglePause()
{
	m_isGamePaused = !m_isGamePaused;
	m_hud.data().drawPause = m_isGamePaused;
}

void PlayState::manageCollision(const Collision& collision)
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

void PlayState::manageBulletEnemyCollision(const Collision& collision)
{
	if (collision.event == CollisionEvent::Enter || collision.event == CollisionEvent::Ongoing)
	{
		const auto& [bullet, enemy] = collision.extractOwners<Bullet, Enemy>();
		m_bulletManager.executeHit(*bullet, *enemy);
	}
}

void PlayState::manageDefenderEnemyCollision(const Collision& collision)
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

void PlayState::manageBaseWallEnemyCollision(const Collision& collision)
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

