#include "Session.h"

#include "GameRegistry.h"

#include <ranges>
#include <raymath.h>

Session::Session(GUI& gui, ResourceSystem& resourceSystem, const GameRegistry& gameRegistry, Config& config, MusicManager& musicManager)
	: m_gameRegistry(gameRegistry)
	, m_config(config)
	, m_defenderManager(m_collisionSystem, musicManager)
	, m_enemyManager(m_gameRegistry, m_collisionSystem, musicManager)
	, m_bulletManager(m_enemyManager, m_collisionSystem, musicManager)
	, m_dropManager(m_gameRegistry, m_collisionSystem)
	, m_defenderPicker(*this, m_gameRegistry)
	, m_levelManager(m_gameRegistry)
	, m_hud(gui, resourceSystem)
	, m_portalManager(m_collisionSystem) {
	m_onEnemiesDestroyedHandle = m_enemyManager.onEnemiesDestroyed(std::bind_front(&Session::onEnemiesDestroyed, this));
	m_onDefenderDestroyedHandle = m_defenderManager.onDefenderDestroyed(std::bind_front(&Session::onDefenderDestroyed, this));
	m_onCollectedDropHandle = m_dropManager.onDropCollected(std::bind_front(&Session::onDropCollected, this));

	m_collisionSystem.addColliderMatch(Collider::Flag::Bullet, Collider::Flag::Enemy);
	m_collisionSystem.addColliderMatch(Collider::Flag::Defender, Collider::Flag::Enemy);
	m_collisionSystem.addColliderMatch(Collider::Flag::BaseWall, Collider::Flag::Enemy);
	m_collisionSystem.addColliderMatch(Collider::Flag::Enemy, Collider::Flag::Portal);
	m_collisionSystem.onCollision([this](const Collision& collision) { manageCollision(collision); });

	m_onLevelManagerActionHandle = m_levelManager.onGameActionRequest([this](const auto& action) { performAction(action); });
}

Session::~Session() {
	m_collisionSystem.destroyCollider(m_baseWall.colliderHandle);
}

bool Session::isState(SessionState state) const {
	return m_gameState == state;
}

void Session::drawGrid() {
	auto& levelData = m_levelManager.getCurrentLevel();
	int columnCount = COLS;
	int rowCount = ROWS;
	for (int row = 0; row < rowCount; row++) {
		DrawLineEx({GRID_OFFSET.x, GRID_OFFSET.y + row * CELL_SIZE}, {GRID_OFFSET.x + columnCount * CELL_SIZE, GRID_OFFSET.y + row * CELL_SIZE}, 1, Fade(BLACK, 0.1f));
	}

	for (int col = 0; col < columnCount; col++) {
		DrawLineEx({GRID_OFFSET.x + col * CELL_SIZE, GRID_OFFSET.y}, {GRID_OFFSET.x + col * CELL_SIZE, GRID_OFFSET.y + rowCount * CELL_SIZE}, 1, Fade(BLACK, 0.1f));
	}

	DrawLineEx({GRID_OFFSET.x + columnCount * CELL_SIZE, GRID_OFFSET.y}, {GRID_OFFSET.x + columnCount * CELL_SIZE, GRID_OFFSET.y + rowCount * CELL_SIZE}, 1, Fade(BLACK, 0.1f));

	DrawLineEx({GRID_OFFSET.x, GRID_OFFSET.y + rowCount * CELL_SIZE}, {GRID_OFFSET.x + columnCount * CELL_SIZE, GRID_OFFSET.y + rowCount * CELL_SIZE}, 1, Fade(BLACK, 0.1f));
}

void Session::update(float dt) {
	if (!m_pauseGameplayLogic) {
		dt *= m_levelData->gameSpeed;
		auto enemyResult = m_enemyManager.update(dt);
		performActions(enemyResult);
		m_collisionSystem.update(dt);

		if (!m_demoMode) {
			performDefenderSpawnOnInput();
		}
		auto result = m_defenderManager.update(dt);
		performActions(result.actions);

		updateBatteryAndScraps(result.amountOfScrapsGain, result.amountOfBatteryDrain);

		m_bulletManager.update(dt);
		m_dropManager.update(dt);
		m_defenderPicker.update(dt);
		m_levelManager.update(dt);
		m_portalManager.update(dt);
	}

	if (DEV_MODE) {
		// When pressing F3 deal 500 damage to a random enemy
		if (IsKeyPressed(KEY_F3)) {
			auto& enemies = m_enemyManager.getEnemies();
			auto filtered = enemies | std::views::transform([](auto& ptr) { return ptr.get(); }) | std::views::filter([](Enemy* item) { return !item->isDying(); });
			if (!filtered.empty()) {
				std::vector<Enemy*> result(filtered.begin(), filtered.end());
				result[Random::range(0, static_cast<int>(result.size()) - 1)]->applyDamage({.value = 500, .source = DamageSource::Bullet});
			}
		}

		// When pressing W win the game
		if (IsKeyPressed(KEY_W)) {
			performAction(WinAction{});
		}
		// When pressing L lose the game
		if (IsKeyPressed(KEY_L)) {
			performAction(LoseAction{});
		}

		// When pressing B add 50 battery
		if (IsKeyPressed(KEY_B)) {
			m_levelData->batteryCharge += 50;
			m_levelData->batteryCharge = Clamp(m_levelData->batteryCharge, 0, m_levelData->info->maxBatteryCharge);
		}

		// When pressing F increase the game speed
		if (IsKeyPressed(KEY_F)) {
			// super hacky way to achieve this and it works only with integer multipliers.
			const std::array speeds = {1, 2, 3, 4, 5};
			int index = m_levelData->gameSpeed;
			if (index >= speeds.size()) {
				index = 0;
			}
			m_levelData->gameSpeed = static_cast<float>(speeds[index]);
		}
	}

	updateHUD(dt);
}

void Session::draw(Atlas& atlas) {
	if (m_gameState != SessionState::Idle) {
		// TODO(Gerark) due to texture bleeding we have to offset the texture. Remove the -1 offset as soon as we solve the issue.
		atlas.drawSprite(m_levelData->info->topBackground, {-1, -1}, 0, Flip::None, WHITE);
		atlas.drawSprite(m_levelData->info->groundBackground, {-1, 63}, 0, Flip::None, {200, 200, 200, 255});

		drawGrid();

		m_portalManager.draw(atlas);
		m_defenderManager.draw(atlas);
		m_enemyManager.draw(atlas);
		m_bulletManager.draw();
		m_dropManager.draw(atlas);
		m_collisionSystem.draw();
	}
}

void Session::setDemoMode(bool demoMode) {
	m_demoMode = demoMode;
}

void Session::setSelectedDefender(const std::optional<std::string>& id) {
	m_selectedDefender = id;
}

void Session::updateBatteryAndScraps(float scrapGain, float batteryDrain) {
	m_levelData->scraps += scrapGain;
	m_levelData->batteryCharge -= batteryDrain;
	m_levelData->batteryCharge = Clamp(m_levelData->batteryCharge, 0, m_levelData->info->maxBatteryCharge);
}

void Session::performDefenderSpawnOnInput() {
	if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))) {
		auto mousePos = GetMousePosition();
		auto [row, column] = getCoordinates(mousePos);

		if (row >= 0 && row < ROWS && column >= 0 && column < COLS) {
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				if (m_selectedDefender) {
					if (canPlaceDefender(row, column)) {
						auto defenderInfo = m_gameRegistry.getDefender(*m_selectedDefender);
						if (defenderInfo && canAffordCost(defenderInfo->cost)) {
							m_defenderManager.spawnDefender(defenderInfo, row, column);
							m_levelData->scraps -= defenderInfo->cost;
							m_defenderPicker.startCooldown(*m_selectedDefender);
							m_levelData->numberOfEnabledDefenders++;
							resetSelectedDefender();
							m_hud.data().occupiedCells.emplace_back(row, column);
						}
					}
				} else if (!canPlaceDefender(row, column)) {
					m_defenderManager.toggleDefender(row, column);
					m_levelData->numberOfEnabledDefenders += m_defenderManager.getDefender(row, column)->state == DefenderState::Off ? -1 : 1;
				}
			}
		}

		if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON) && m_selectedDefender) {
			resetSelectedDefender();
		}
	}
}

void Session::performActions(const GameActions& actions) {
	for (auto& action : actions) {
		performAction(action);
	}
}

void Session::performAction(const GameAction& action) {
	std::visit([this](auto&& action) { performAction(action); }, action);
}

void Session::performAction(const BulletSpawnAction& action) {
	auto bulletInfo = m_gameRegistry.getBullet(action.bulletType);
	if (bulletInfo) {
		m_bulletManager.spawnBullet(*bulletInfo, action.position);
	}
}

void Session::performAction(const EnemySpawnAction& action) {
	auto enemyInfo = m_gameRegistry.getEnemy(action.enemyType);
	m_enemyManager.spawnEnemy(enemyInfo, action.row, action.column);
	m_levelData->enemyCount++;
}

void Session::performAction(const DefenderSpawnAction& action) {
	auto defenderInfo = m_gameRegistry.getDefender(action.id);
	if (defenderInfo) {
		auto& defender = m_defenderManager.spawnDefender(defenderInfo, action.row, action.column);
		if (action.isEnabled) {
			m_defenderManager.setState(defender, DefenderState::On);
			m_levelData->numberOfEnabledDefenders++;
		} else {
			m_defenderManager.setState(defender, DefenderState::Off);
		}
	}
}

void Session::performAction(const WinAction& action) {
	if (m_demoMode) {
		m_hud.startFadeInOut([this]() { restartSession(); }, []() {}, 0.5f);
		return;
	}

	if (m_levelManager.isLastLevel()) {
		setState(SessionState::End);
	} else {
		setState(SessionState::Win);
	}
}

void Session::performAction(const LoseAction& action) {
	if (m_demoMode) {
		m_hud.startFadeInOut([this]() { restartSession(); }, []() {}, 0.5f);
		return;
	}

	setState(SessionState::Lost);
}

void Session::performAction(const MessageAction& action) {
	auto& data = m_hud.data();
	data.showMessage = true;
	data.messageTime = 0;
	data.messageAction = action;
}

void Session::performAction(const HUDAction& action) {
	switch (action.type) {
	case HUDOperationType::Enable			 : m_hud.setEnable(true); break;
	case HUDOperationType::Disable			 : m_hud.setEnable(false); break;
	case HUDOperationType::ShowResources	 : m_hud.data().showResources = true; break;
	case HUDOperationType::HideResources	 : m_hud.data().showResources = false; break;
	case HUDOperationType::ShowDefenderPicker: m_hud.data().showDefenderPicker = true; break;
	case HUDOperationType::HideDefenderPicker: m_hud.data().showDefenderPicker = false; break;
	case HUDOperationType::ShowTimeline		 : m_hud.data().showTimeline = true; break;
	case HUDOperationType::HideTimeline		 : m_hud.data().showTimeline = false; break;
	}
}

void Session::performAction(const DefenderPickerAction& action) {
	switch (action.type) {
	case DefenderPickerOperationType::AddItem: m_defenderPicker.addPickableItem(action.id); break;
	case DefenderPickerOperationType::Reset	 : m_defenderPicker.reset(); break;
	}
	refreshHUDDefenderPickerData();
}

void Session::performAction(const std::monostate& action) {
	// No action to perform
}

void Session::performAction(const PortalSpawnAction& action) {
	auto entrance = m_gameRegistry.getPortal("entrance");
	auto exit = m_gameRegistry.getPortal("exit");
	m_portalManager.spawnPortals(entrance, exit, action.inRow, action.inCol, action.outRow, action.outCol);
}

bool Session::canAffordCost(int cost) const {
	return cost <= m_levelData->scraps;
}

bool Session::canPlaceDefender(int x, int y) const {
	return !m_defenderManager.hasDefender(x, y);
}

void Session::setupHUD() {
	auto& hudData = m_hud.data();
	refreshHUDDefenderPickerData();

	hudData.levelName = m_levelData->info->name;
	hudData.maxBatteryCharge = m_levelData->info->maxBatteryCharge;
	hudData.showDefenderPicker = !m_demoMode;
	hudData.showResources = !m_demoMode;
	hudData.showTimeline = !m_demoMode;
	m_onDefenderSelectedCallbackHandle = m_hud.onDefenderSelected([this](const auto& index) { setSelectedDefender(m_hud.data().pickableDefenders[index].id); });

	hudData.timelineData.duration = m_levelData->info->timeline.keyframes.back().time;
	hudData.timelineData.time = m_levelData->time;
	for (const auto& keyframe : m_levelData->info->timeline.keyframes) {
		if (std::holds_alternative<FlagTimelineOperation>(keyframe.action)) {
			auto& flag = std::get<FlagTimelineOperation>(keyframe.action);
			hudData.timelineData.waves.emplace_back(keyframe.time / hudData.timelineData.duration, flag.icon);
		}
	}
}

void Session::manageCollision(const Collision& collision) {
	switch (collision.mask) {
	case cMask(Collider::Flag::Bullet, Collider::Flag::Enemy)  : manageBulletEnemyCollision(collision); break;
	case cMask(Collider::Flag::Defender, Collider::Flag::Enemy): manageDefenderEnemyCollision(collision); break;
	case cMask(Collider::Flag::BaseWall, Collider::Flag::Enemy): manageBaseWallEnemyCollision(collision); break;
	case cMask(Collider::Flag::Enemy, Collider::Flag::Portal)  : manageEnemyPortalCollision(collision); break;
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
			m_levelData->batteryCharge -= static_cast<int>(enemy->getInfo()->baseWallDamage);
			enemy->setSparkEffect(1.f);
			enemy->applyDamage({50, false, DamageSource::BaseWall});
			enemy->setState(EnemyState::PrepareToAttack);
		} else if (enemy->getState() != EnemyState::PrepareToAttack) {
			enemy->setState(EnemyState::PrepareToAttack);
		}
		break;
	}
}

void Session::manageEnemyPortalCollision(const Collision& collision) {
	const auto& [portal, enemy] = collision.extractOwners<Portal, Enemy>();
	if (enemy->isDying()) {
		return;
	}

	if (portal->type == PortalType::Entrance) {
		switch (collision.event) {
		case CollisionEvent::Enter:
			Vector2 exitPosition = m_portalManager.getExit(portal->id)->position;
			enemy->setPosition(exitPosition);
			break;
		case CollisionEvent::Exit	: break;
		case CollisionEvent::Ongoing: break;
		}
	}
}

void Session::resetSelectedDefender() {
	m_selectedDefender.reset();
	m_hud.data().selectedDefenderIndex.reset();
}

void Session::restartSession() {
	resetProgression();
	startNextLevel();
}

void Session::resetProgression() {
	clearAllEntities();
	m_levelManager.resetCurrentLevelIndex();

	m_selectedDefender.reset();
	m_defenderPicker.reset();
	m_pauseGameplayLogic = true;
	m_hud.clear();
	m_hud.setEnable(false);
	m_hud.setVisible(false);

	if (m_demoMode) {
		m_levelManager.setLevelSequence({"demoLevel"});
	} else {
		std::vector<std::string> levelSquence = {"level1", "level2", "level3"};
		if (m_config.options.isTutorialEnabled) {
			levelSquence.insert(levelSquence.begin(), "tutorial");
		}
		m_levelManager.setLevelSequence(std::move(levelSquence));
	}
}

void Session::startNextLevel() {
	// If we're starting a new level and the current level has been the tutorial we disable it.
	// It translates into: "If you played the tutorial and you're progressing to the next level, we don't want to show the tutorial again".
	if (m_levelManager.getCurrentLevel().info && m_levelManager.getCurrentLevel().info->id == "tutorial") {
		m_config.options.isTutorialEnabled = false;
	}

	m_pauseGameplayLogic = false;
	m_hud.clear();
	m_hud.setEnable(!m_demoMode);
	m_hud.setVisible(true);
	clearAllEntities();
	m_collisionSystem.destroyCollider(m_baseWall.colliderHandle);
	m_collisionSystem.clearColliders();

	m_levelData = m_levelManager.startNextLevel();
	m_baseWall.colliderHandle = m_collisionSystem.createCollider(Collider::Flag::BaseWall, &m_baseWall);
	m_collisionSystem.updateCollider(m_baseWall.colliderHandle, {GRID_OFFSET.x - 5, GRID_OFFSET.y, 5, CELL_SIZE * ROWS});
	setupHUD();
	setState(SessionState::Playing);
}

void Session::pause() {
	setState(SessionState::Paused);
}

void Session::resume() {
	setState(SessionState::Playing);
}

void Session::clearAllEntities() {
	m_portalManager.clear();
	m_defenderManager.clear();
	m_enemyManager.clear();
	m_dropManager.clear();
	m_bulletManager.clear();
}

void Session::setState(SessionState state) {
	m_gameState = state;

	switch (state) {
	case SessionState::Playing: m_hud.setEnable(!m_demoMode); break;
	case SessionState::Idle	  :
	case SessionState::Win	  :
	case SessionState::End	  :
	case SessionState::Lost	  :
	case SessionState::Paused : m_hud.setEnable(false); break;
	}
}

void Session::updateHUD(float dt) {
	auto& hudData = m_hud.data();
	hudData.batteryCharge = m_levelManager.getCurrentLevel().batteryCharge;
	hudData.scrapsAmount = static_cast<int>(m_levelManager.getCurrentLevel().scraps);

	// TODO(Gerark) - Not very optimal but we'll see if it's going to ever be a bottleneck.
	hudData.progressBars.clear();
	hudData.deployedDefenders.clear();
	for (auto& defender : getDefenderManager().getDefenders()) {
		if (defender->state == DefenderState::Dying || defender->state == DefenderState::Dead) {
			continue;
		}
		hudData.progressBars.push_back(ProgressBarData{.value = static_cast<float>(defender->hp), .max = static_cast<float>(defender->info->maxHP), .position = defender->position});
		hudData.deployedDefenders.emplace_back(defender->state, defender->position);
	}

	for (auto& enemy : getEnemyManager().getEnemies()) {
		if (enemy->isDying()) {
			continue;
		}
		hudData.progressBars.push_back(ProgressBarData{.value = enemy->getHp(), .max = enemy->getInfo()->maxHp, .position = enemy->getPosition()});
	}

	for (auto& hudDefender : hudData.pickableDefenders) {
		auto* defender = m_gameRegistry.getDefender(hudDefender.id);
		auto* bullet = defender->bulletType ? m_gameRegistry.getBullet(*defender->bulletType) : nullptr;
		auto& pickableDefender = m_defenderPicker.getDefender(hudDefender.id);
		hudDefender.cost = pickableDefender.cost;
		hudDefender.cooldown = pickableDefender.currentCooldown;
		hudDefender.maxCooldown = pickableDefender.maxCooldown;
		hudDefender.batteryDrain = defender->batteryDrain;

		hudDefender.damage = bullet ? std::visit([](auto&& arg) { return arg.damage; }, *bullet) : DamageInfo{};
		hudDefender.canAfford = m_defenderPicker.canAfford(hudDefender.id);
	}

	hudData.timelineData.time = m_levelData->time;

	m_hud.update(dt);
}

void Session::refreshHUDDefenderPickerData() {
	auto& hudData = m_hud.data();
	hudData.pickableDefenders.clear();
	for (const auto& pickableDefender : m_defenderPicker.getAvailableDefenders()) {
		auto defenderInfo = m_gameRegistry.getDefender(pickableDefender.id);
		hudData.pickableDefenders.emplace_back(defenderInfo->name, pickableDefender.id, Animation::createAnimation(defenderInfo->spriteEnabled), defenderInfo->cost);
	}
}

void Session::onEnemiesDestroyed(const std::vector<EnemyDestroyedInfo>& enemies) {
	m_levelData->enemyCount -= static_cast<int>(enemies.size());
	assert(m_levelData->enemyCount >= 0);

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

void Session::onDefenderDestroyed(Defender& defender) {
	std::erase_if(m_hud.data().occupiedCells, [&](const auto& cell) { return cell.row == defender.row && cell.column == defender.column; });
	if (defender.state == DefenderState::On) {
		m_levelData->numberOfEnabledDefenders--;
	}
}

void Session::onDropCollected(const std::vector<CollectedDrop>& collectedDrops) {
	for (const auto& drop : collectedDrops) {
		if (drop.info->type == DropType::Scraps) {
			m_levelData->scraps += drop.amount;
		}
	}
}
