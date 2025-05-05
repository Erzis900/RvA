#include "Game.h"
#include "Session.h"

#include <raymath.h>

Session::Session(GUI& gui, const EnemyTypeRegistry& enemyTypeRegistry, const DefenderTypeRegistry& defenderTypeRegistry, const BulletTypeRegistry& bulletTypeRegistry)
	: m_enemyTypeRegistry(enemyTypeRegistry)
    , m_defenderTypeRegistry(defenderTypeRegistry)
	, m_bulletTypeRegistry(bulletTypeRegistry)
	, m_defenderManager(m_collisionSystem)
	, m_enemyManager(enemyTypeRegistry, m_collisionSystem)
	, m_bulletManager(m_enemyManager, m_collisionSystem)
	, m_hud(gui)
{
	m_enemyManager.onEnemiesDestroyed([this](int numberOfDestroyedEnemies) {
		m_numberOfDestroyedEnemies += numberOfDestroyedEnemies;
	});

	m_collisionSystem.addColliderMatch(Collider::Flag::Bullet, Collider::Flag::Enemy);
	m_collisionSystem.addColliderMatch(Collider::Flag::Defender, Collider::Flag::Enemy);
	m_collisionSystem.addColliderMatch(Collider::Flag::BaseWall, Collider::Flag::Enemy);
	m_collisionSystem.onCollision([this](const Collision& collision) { manageCollision(collision); });
}

Session::~Session()
{
	m_collisionSystem.destroyCollider(m_baseWall.colliderHandle);
}

void Session::setPause(bool paused)
{
	m_isPaused = paused;
    m_hud.setEnable(!paused);
}

bool Session::isPaused() const
{
	return m_isPaused;
}

void Session::start()
{
	m_isStarted = true;

	setupHUD();
	m_hud.setEnable(true);
	m_hud.setVisible(true);

	if (m_isPaused)
	{
		setPause(false);
	}
	else
	{
		m_baseWall.colliderHandle = m_collisionSystem.createCollider(Collider::Flag::BaseWall, &m_baseWall);
		m_collisionSystem.updateCollider(
			m_baseWall.colliderHandle,
			{ GRID_OFFSET.x - 5, GRID_OFFSET.y, 5, CELL_SIZE * ROWS }
		);
	}
}

void Session::end()
{
	m_isStarted = false;
	m_isPaused = false;
    m_defenderManager.clear();
    m_enemyManager.clear();
    m_bulletManager.clear();
    m_collisionSystem.destroyCollider(m_baseWall.colliderHandle);
	m_collisionSystem.clearColliders();
    m_numberOfDestroyedEnemies = 0;
    m_batteryCharge = MAX_BATTERY_CHARGE;
    m_scraps = 0;
    m_selectedDefender.reset();
	m_hud.clear();
	m_hud.setVisible(false);
}

void Session::drawGrid()
{
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			int x = GRID_OFFSET.x + col * CELL_SIZE;
			int y = GRID_OFFSET.y + row * CELL_SIZE;

			DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, BLACK);
		}
	}
}

void Session::update(float dt)
{
	m_enemyManager.update(dt);
	m_collisionSystem.update(dt);

	performDefenderSpawnOnInput();
	auto result = m_defenderManager.update(dt);
	performActions(result.actions);
	updateBatteryAndScraps(result.amountOfScrapsGain, result.amountOfBatteryDrain);

	m_bulletManager.update(dt);

	updateHUD(dt);
}

void Session::draw(Atlas& atlas)
{
	if(m_isStarted)
	{
		drawGrid();

		m_defenderManager.draw(atlas);
		m_enemyManager.draw(atlas);
		m_bulletManager.draw();
		m_collisionSystem.draw();
	}
}

void Session::setSelectedDefender(std::optional<DefenderType> type)
{
	m_selectedDefender = type;
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
					auto defenderInfo = m_defenderTypeRegistry.getDefenderInfo(*m_selectedDefender);
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
	auto bulletInfo = m_bulletTypeRegistry.getBulletInfo(action.bulletType);
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

void Session::setupHUD()
{
	auto& hudData = m_hud.data();
	hudData.defenders.clear();
	for (const auto& [type, defenderInfo] : m_defenderTypeRegistry.getDefenderInfos()) {
		hudData.defenders.emplace_back(type, defenderInfo.spriteEnabled.spriteInfo, defenderInfo.cost);
	}

	m_onDefenderSelectedCallbackHandle = m_hud.onDefenderSelected([this]() { setSelectedDefender(m_hud.data().selectedDefender); });
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

void Session::updateHUD(float dt) {
	auto& hudData = m_hud.data();
	hudData.batteryCharge = getBatteryCharge();
	hudData.scrapsAmount = static_cast<int>(getScraps());

	// TODO(Gerark) - Not very optimal but we'll see if it's going to ever be a bottleneck.
	hudData.progressBars.clear();
	for (auto& defender : getDefenderManager().getDefenders()) {
		hudData.progressBars.push_back(ProgressBarData{
			.value = static_cast<float>(defender->hp),
			.max = static_cast<float>(defender->info->maxHP),
			.position = defender->position,
			.bkgColor = DARKGRAY,
			.fillColor = GREEN
			});
	}

	for (auto& enemy : getEnemyManager().getEnemies()) {
		hudData.progressBars.push_back(ProgressBarData{
			.value = enemy->getHp(),
			.max = enemy->getInfo()->maxHp,
			.position = enemy->getPosition(),
			.bkgColor = DARKGRAY,
			.fillColor = GREEN
			});
	}

	m_hud.update(dt);
}