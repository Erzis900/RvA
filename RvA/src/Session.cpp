#include "Session.h"

#include "Game.h"
#include "GameRegistry.h"

#include <ranges>
#include <raymath.h>

Session::Session(GUI& gui, const GameRegistry& gameRegistry)
	: m_gameRegistry(gameRegistry)
	, m_defenderManager(m_collisionSystem)
	, m_enemyManager(m_gameRegistry, m_collisionSystem)
	, m_bulletManager(m_enemyManager, m_collisionSystem)
	, m_dropManager(m_gameRegistry, m_collisionSystem)
	, m_defenderPicker(*this, m_gameRegistry)
	, m_hud(gui) {
	m_onEnemiesDestroyedHandle = m_enemyManager.onEnemiesDestroyed(std::bind_front(&Session::onEnemiesDestroyed, this));
	m_onDefenderDestroyedHandle = m_defenderManager.onDefenderDestroyed(
		[this](int row, int column) { std::erase_if(m_hud.data().occupiedCells, [row, column](const auto& cell) { return cell.row == row && cell.column == column; }); });
	m_onCollectedDropHandle = m_dropManager.onDropCollected(std::bind_front(&Session::onDropCollected, this));

	m_collisionSystem.addColliderMatch(Collider::Flag::Bullet, Collider::Flag::Enemy);
	m_collisionSystem.addColliderMatch(Collider::Flag::Defender, Collider::Flag::Enemy);
	m_collisionSystem.addColliderMatch(Collider::Flag::BaseWall, Collider::Flag::Enemy);
	m_collisionSystem.onCollision([this](const Collision& collision) { manageCollision(collision); });
}

Session::~Session() {
	m_collisionSystem.destroyCollider(m_baseWall.colliderHandle);
}

void Session::setPause(bool paused) {
	m_isPaused = paused;
	m_hud.setEnable(!paused);
}

bool Session::isPaused() const {
	return m_isPaused;
}

void Session::start() {
	m_isStarted = true;

	m_hud.setEnable(true);
	m_hud.setVisible(true);

	if (m_isPaused) {
		setPause(false);
	} else {
		m_scraps = 100; // TODO(Gerark) - We'll have a configuration to define what's the starting amount of scraps
		m_defenderPicker.reset();
		m_baseWall.colliderHandle = m_collisionSystem.createCollider(Collider::Flag::BaseWall, &m_baseWall);
		m_collisionSystem.updateCollider(m_baseWall.colliderHandle, {GRID_OFFSET.x - 5, GRID_OFFSET.y, 5, CELL_SIZE * ROWS});
		setupHUD();
	}
}

void Session::end() {
	m_isStarted = false;
	m_isPaused = false;
	m_defenderManager.clear();
	m_enemyManager.clear();
	m_dropManager.clear();
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

void Session::drawGrid() {
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			int x = static_cast<int>(GRID_OFFSET.x) + col * CELL_SIZE;
			int y = static_cast<int>(GRID_OFFSET.y) + row * CELL_SIZE;

			DrawRectangleLines(x, y, CELL_SIZE, CELL_SIZE, BLACK);
		}
	}
}

void Session::update(float dt) {
	m_enemyManager.update(dt);
	m_collisionSystem.update(dt);

	performDefenderSpawnOnInput();
	auto result = m_defenderManager.update(dt);
	performActions(result.actions);
	updateBatteryAndScraps(result.amountOfScrapsGain, result.amountOfBatteryDrain);

	m_bulletManager.update(dt);
	m_dropManager.update(dt);
	m_defenderPicker.update(dt);

	// When pressing F3 deal 500 damage to a random enemy
	if (DEV_MODE && IsKeyPressed(KEY_F3)) {
		auto& enemies = m_enemyManager.getEnemies();
		auto filtered = enemies | std::views::transform([](auto& ptr) { return ptr.get(); }) | std::views::filter([](Enemy* item) { return !item->isDying(); });
		if (!filtered.empty()) {
			std::vector<Enemy*> result(filtered.begin(), filtered.end());
			result[Random::range(0, static_cast<int>(result.size()) - 1)]->applyDamage({.value = 500, .source = DamageSource::Bullet});
		}
	}

	updateHUD(dt);
}

void Session::draw(Atlas& atlas) {
	if (m_isStarted) {
		drawGrid();

		m_defenderManager.draw(atlas);
		m_enemyManager.draw(atlas);
		m_bulletManager.draw();
		m_dropManager.draw(atlas);
		m_collisionSystem.draw();
	}
}

void Session::setSelectedDefender(std::optional<DefenderType> type) {
	m_selectedDefender = type;
}

void Session::updateBatteryAndScraps(float scrapGain, float batteryDrain) {
	m_scraps += scrapGain;
	m_batteryCharge -= batteryDrain;
	m_batteryCharge = Clamp(m_batteryCharge, 0, 100);
}

void Session::performDefenderSpawnOnInput() {
	if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))) {
		auto mousePos = GetMousePosition();
		auto [row, column] = getCoordinates(mousePos);

		if (row >= 0 && row < ROWS && column >= 0 && column < COLS) {
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && m_selectedDefender) {
				if (canPlaceDefender(row, column)) {
					auto defenderInfo = m_gameRegistry.getDefender(*m_selectedDefender);
					if (defenderInfo && canAffordCost(defenderInfo->cost)) {
						m_defenderManager.spawnDefender(defenderInfo, row, column);
						m_scraps -= defenderInfo->cost;
						m_defenderPicker.startCooldown(*m_selectedDefender);
						resetSelectedDefender();
						m_hud.data().occupiedCells.emplace_back(row, column);
					}
				}
			} else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
				if (!canPlaceDefender(row, column)) {
					m_defenderManager.toggleDefender(row, column);
				}
			}
		}

		if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && m_selectedDefender) {
			resetSelectedDefender();
		}
	}
}

void Session::performActions(const Actions& actions) {
	for (auto& action : actions) {
		std::visit([this](auto&& action) { performAction(action); }, action);
	}
}

void Session::performAction(const BulletSpawnAction& action) {
	auto bulletInfo = m_gameRegistry.getBullet(action.bulletType);
	if (bulletInfo) {
		m_bulletManager.spawnBullet(*bulletInfo, action.position);
	}
}

bool Session::canAffordCost(int cost) const {
	return cost <= m_scraps;
}

bool Session::canPlaceDefender(int x, int y) const {
	return !m_defenderManager.hasDefender(x, y);
}

void Session::setupHUD() {
	auto& hudData = m_hud.data();
	hudData.defenders.clear();
	for (const auto& [type, pickableDefender] : m_defenderPicker.getAvailableDefenders()) {
		auto defenderInfo = m_gameRegistry.getDefender(type);
		hudData.defenders.emplace_back(type, defenderInfo->spriteEnabled.spriteInfo, defenderInfo->cost);
	}

	m_onDefenderSelectedCallbackHandle = m_hud.onDefenderSelected([this](const auto& index) { setSelectedDefender(m_hud.data().defenders[index].type); });
}

void Session::manageCollision(const Collision& collision) {
	switch (collision.mask) {
	case cMask(Collider::Flag::Bullet, Collider::Flag::Enemy)  : manageBulletEnemyCollision(collision); break;
	case cMask(Collider::Flag::Defender, Collider::Flag::Enemy): manageDefenderEnemyCollision(collision); break;
	case cMask(Collider::Flag::BaseWall, Collider::Flag::Enemy): manageBaseWallEnemyCollision(collision); break;
	}
}

void Session::manageBulletEnemyCollision(const Collision& collision) {
	if (collision.event == CollisionEvent::Enter || collision.event == CollisionEvent::Ongoing) {
		const auto& [bullet, enemy] = collision.extractOwners<Bullet, Enemy>();
		m_bulletManager.executeHit(*bullet, *enemy);
	}
}

void Session::manageDefenderEnemyCollision(const Collision& collision) {
	const auto& [defender, enemy] = collision.extractOwners<Defender, Enemy>();
	if (enemy->isDying()) {
		return;
	}

	switch (collision.event) {
	case CollisionEvent::Enter: enemy->setState(EnemyState::PrepareToAttack); break;
	case CollisionEvent::Exit : enemy->setState(EnemyState::Moving); break;
	case CollisionEvent::Ongoing:
		if (enemy->getState() == EnemyState::ReadyToAttack) {
			defender->hp -= static_cast<int>(enemy->getInfo()->defenderDamage);
			enemy->setState(EnemyState::PrepareToAttack);
		} else if (enemy->getState() != EnemyState::PrepareToAttack) {
			enemy->setState(EnemyState::PrepareToAttack);
		}
		break;
	}
}

void Session::manageBaseWallEnemyCollision(const Collision& collision) {
	const auto& [defender, enemy] = collision.extractOwners<Defender, Enemy>();
	if (enemy->isDying()) {
		return;
	}

	switch (collision.event) {
	case CollisionEvent::Enter: enemy->setState(EnemyState::PrepareToAttack); break;
	case CollisionEvent::Exit : break;
	case CollisionEvent::Ongoing:
		if (enemy->getState() == EnemyState::ReadyToAttack) {
			m_batteryCharge -= static_cast<int>(enemy->getInfo()->baseWallDamage);
			enemy->applyDamage({50, false, DamageSource::BaseWall});
			enemy->setState(EnemyState::PrepareToAttack);
		} else if (enemy->getState() != EnemyState::PrepareToAttack) {
			enemy->setState(EnemyState::PrepareToAttack);
		}
		break;
	}
}

void Session::resetSelectedDefender() {
	m_selectedDefender.reset();
	m_hud.data().selectedDefenderIndex.reset();
}

void Session::updateHUD(float dt) {
	auto& hudData = m_hud.data();
	hudData.batteryCharge = getBatteryCharge();
	hudData.scrapsAmount = static_cast<int>(getScraps());

	// TODO(Gerark) - Not very optimal but we'll see if it's going to ever be a bottleneck.
	hudData.progressBars.clear();
	for (auto& defender : getDefenderManager().getDefenders()) {
		hudData.progressBars.push_back(
			ProgressBarData{.value = static_cast<float>(defender->hp), .max = static_cast<float>(defender->info->maxHP), .position = defender->position, .bkgColor = DARKGRAY, .fillColor = GREEN});
	}

	for (auto& enemy : getEnemyManager().getEnemies()) {
		hudData.progressBars.push_back(ProgressBarData{.value = enemy->getHp(), .max = enemy->getInfo()->maxHp, .position = enemy->getPosition(), .bkgColor = DARKGRAY, .fillColor = GREEN});
	}

	for (auto& hudDefender : hudData.defenders) {
		auto& pickableDefender = m_defenderPicker.getDefender(hudDefender.type);
		hudDefender.cost = pickableDefender.cost;
		hudDefender.cooldown = pickableDefender.currentCooldown;
		hudDefender.maxCooldown = pickableDefender.maxCooldown;
		hudDefender.canAfford = m_defenderPicker.canAfford(hudDefender.type);
	}

	m_hud.update(dt);
}

void Session::onEnemiesDestroyed(const std::vector<EnemyDestroyedInfo>& enemies) {
	m_numberOfDestroyedEnemies += static_cast<int>(enemies.size());

	for (const auto& enemy : enemies) {
		if (enemy.damageSource == DamageSource::Bullet) {
			if (enemy.info->dropType) {
				auto* drop = m_gameRegistry.getDrop(*enemy.info->dropType);
				auto amount = enemy.info->dropAmount.generate();
				m_dropManager.spawnDrop(drop, enemy.position, amount);
			}
		}
	}
}

void Session::onDropCollected(const std::vector<CollectedDrop>& collectedDrops) {
	for (const auto& drop : collectedDrops) {
		if (drop.info->type == DropType::Scraps) {
			m_scraps += drop.amount;
		}
	}
}
