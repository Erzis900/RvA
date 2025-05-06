#include "Game.h"

#include "constants.h"
#include "fsm/FsmBuilder.h"
#include "states/CreditsState.h"
#include "states/LostState.h"
#include "states/MenuState.h"
#include "states/OptionsState.h"
#include "states/PauseState.h"
#include "states/PlayState.h"
#include "states/WinState.h"

Game::Game()
	: m_renderRec()
	, m_scale(1.f)
	, m_texWidth(TEX_WIDTH)
	, m_texHeight(TEX_HEIGHT)
	, m_screenWidth(SCREEN_WIDTH)
	, m_screenHeight(SCREEN_HEIGHT)
	, m_gui(m_atlas, m_musicManager)
	, m_gameSession(m_gui, m_enemyTypeRegistry, m_defenderTypeRegistry, m_bulletTypeRegistry) {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(m_screenWidth, m_screenHeight, "RvA");
	SetTargetFPS(60);

	m_renderTexture = LoadRenderTexture(m_texWidth, m_texHeight);
	SetTextureFilter(m_renderTexture.texture, TEXTURE_FILTER_POINT);

	m_atlas.load("assets/atlas.png");
	m_gui.loadResources();

	DisableCursor();
	m_gui.setCursor(CursorType::Point);

	registerDefenderTypes();
	registerBulletTypes();
	registerEnemyTypes();

	InitAudioDevice();
	m_musicManager.load();
	m_gui.setDefaultButtonSound(&m_musicManager.getButtonClick());

	setupFSM();

	m_currentState = std::make_unique<MenuState>(*this);
	m_currentState->onEnter(*this);

	SetExitKey(0);
}

Game::~Game() {
	UnloadRenderTexture(m_renderTexture);
	CloseAudioDevice();
	CloseWindow();
}

void Game::update() {
	float dt = GetFrameTime();
	updateRenderRec();
	updateMouse();

	m_fsm->update(dt);

	if (DEV_MODE && IsKeyPressed(KEY_F2)) {
		m_gui.toggleDebugView();
	}

	m_gui.update(dt);
	m_musicManager.updateStream();
}

void Game::updateRenderRec() {
	m_screenWidth = GetScreenWidth();
	m_screenHeight = GetScreenHeight();

	m_scale = std::min(float(m_screenWidth) / float(m_texWidth), float(m_screenHeight) / float(m_texHeight));
	m_renderRec.width = m_texWidth * m_scale;
	m_renderRec.height = m_texHeight * m_scale;
	m_renderRec.x = (m_screenWidth - m_renderRec.width) / 2.f;
	m_renderRec.y = (m_screenHeight - m_renderRec.height) / 2.f;
}

void Game::updateMouse() {
	int offsetX = int(-(m_screenWidth - (m_texWidth * m_scale)) / 2.f);
	int offsetY = int(-(m_screenHeight - (m_texHeight * m_scale)) / 2.f);

	SetMouseOffset(offsetX, offsetY);
	SetMouseScale(1 / m_scale, 1 / m_scale);
}

void Game::draw() {
	BeginTextureMode(m_renderTexture);

	ClearBackground(GRAY);

	m_currentState->draw(*this);
	m_gameSession.draw(m_atlas);
	m_drawCallbacks.executeCallbacks();

	m_gui.draw();

	EndTextureMode();

	BeginDrawing();
	ClearBackground(BLACK);

	DrawTexturePro(m_renderTexture.texture, {0.f, 0.f, float(m_texWidth), -float(m_texHeight)}, m_renderRec, {0.f, 0.f}, 0.f, WHITE);
	EndDrawing();
}

/* Keeping this commented as a reference for the fade/transition to be restored
void Game::internalSetState(std::unique_ptr<IGameState> newState, bool useFade)
{
	m_currentState->onExit(*this);

	if (useFade)
	{
		m_nextState = std::move(newState);

		m_gui.startFadingInOut([this] {
			m_currentState = std::move(m_nextState);
			m_currentState->onEnter(*this);
		}, [this] { m_isTransitionInProgress = false; }, 0.5f);
		m_isTransitionInProgress = true;
	}
	else
	{
		m_currentState = std::move(newState);
		m_currentState->onEnter(*this);
	}
}
*/

bool Game::shouldClose() const {
	return WindowShouldClose() || m_fsm->hasReachedExitState();
}

void Game::setupFSM() {
	auto fsmBuilder = flow::FsmBuilder();

	// clang-format off
	fsmBuilder
		// Menus
		.state<MenuState>("MainMenu", *this)
			.on("play").jumpTo("Play")
			.on("options").jumpTo("Options")
			.on("credits").jumpTo("Credits")
			.on("exit").exitFsm()

		.state<OptionsState>("Options", *this)
			.on("back").jumpTo("MainMenu")

		.state<CreditsState>("Credits", *this)
			.on("back").jumpTo("MainMenu")

		// In Game States
		.state<PlayState>("Play", *this)
			.on("pause").jumpTo("PauseMenu")
			.on("win").jumpTo("WinScreen")
			.on("lost").jumpTo("LoseScreen")

		.state<PauseState>("PauseMenu", *this)
			.on("resume").jumpTo("Play")
			.on("restart").jumpTo("Play")
			.on("menu").jumpTo("MainMenu")

		.state<WinState>("WinScreen", *this)
			.on("menu").jumpTo("MainMenu")

		.state<LostState>("LoseScreen", *this)
			.on("restart").jumpTo("Play")
			.on("menu").jumpTo("MainMenu");
	// clang-format on

	auto [fsm, fsmInfo] = fsmBuilder.build("MainMenu", nullptr);
	m_fsm = std::move(fsm);
}

void Game::run() {
	while (!shouldClose()) {
		update();
		draw();
	}
}

void Game::registerDefenderTypes() {
	auto sprite = [this](const char* spriteName) { return m_atlas.getSpriteInfo(spriteName); };

	m_defenderTypeRegistry.registerDefender({
		.type = DefenderType::Solar,
		.spriteEnabled = {sprite("solar_idle"), 0.1f},
		.spriteDisabled = {sprite("solar_off"), 0.1f},
		.batteryDrain = -5,
		.scrapsGain = 5,
		.maxHP = 100,
	});

	m_defenderTypeRegistry.registerDefender({
		.type = DefenderType::Shooter,
		.spriteEnabled = { sprite("shooter_idle"), 0.1f },
		.spriteDisabled = { sprite("shooter_off"), 0.1f },
		.spriteShoot = {sprite("shooter_shoot"), 0.1f},
		.batteryDrain = 5.f,
		.firstShootCooldown = 3.f,
		.shootCooldown = 2.f,
		.maxHP = 150,
		.cost = 10,
		.bulletType = "SimpleShot",
		.shootingAnimationTime = 0.8f
	});

	m_defenderTypeRegistry.registerDefender({
		.type = DefenderType::Catapult,
		.spriteEnabled = { sprite("catapult_idle"), 0.1f },
		.spriteDisabled = { sprite("catapult_off"), 0.1f },
		.spriteShoot = {sprite("catapult_shoot"), 0.1f},
		.batteryDrain = 10.f,
		.firstShootCooldown = 3.f,
		.shootCooldown = 1.f,
		.maxHP = 200,
		.cost = 20,
		.bulletType = "ChasingShot",
		.shootingAnimationTime = 0.7f
	});

	m_defenderTypeRegistry.registerDefender({
		.type = DefenderType::Lasertron,
		.spriteEnabled = { sprite("lasertron_idle"), 0.1f },
		.spriteDisabled = { sprite("lasertron_off"), 0.1f },
		.spriteShoot = {sprite("lasertron_shoot"), 0.1f},
		.batteryDrain = 20.f,
		.firstShootCooldown = 3.f,
		.shootCooldown = 2.f,
		.maxHP = 250,
		.cost = 30,
		.bulletType = "LaserBeam",
		.shootingAnimationTime = 1.2f
	});
}

void Game::registerBulletTypes() {
	m_bulletTypeRegistry.registerBulletType("SimpleShot",
											BulletShotData{
												.velocity = {150, 0},
												.radius = 5.f,
												.damage = {25, 16},
												.maxLifetime = 100,
											});

	m_bulletTypeRegistry.registerBulletType("ChasingShot",
											ChasingShotData{
												.startOffset = {20, 20},
												.radius = 10.f,
												.damage = {50, 16},
												.maxLifetime = 100,
												.speed = 150,
												.color = {255, 0, 0, 255},
												.direction = {1, 0},
											});

	m_bulletTypeRegistry.registerBulletType("LaserBeam",
											LaserBeamData{
												.startOffset = {35, 18},
												.beamHeight = 2,
												.damage = {100.f, 0, true},
												.auraSize = 2,
												.beamColor = BLUE,
												.auraColor = {255, 255, 255, 200},
												.maxLifetime = 0.5f,
												.shootAnimationSpeed = 15,
												.shootAnimationDuration = 2.f,
											});
}

void Game::registerEnemyTypes() {
	auto sprite = [this](const char* spriteName) { return m_atlas.getSpriteInfo(spriteName); };

	m_enemyTypeRegistry.registerEnemyType({.type = EnemyType::B1,
										   .spawnChance = 0.2f,
										   .maxHp = 100,
										   .speed = 40,
										   .attackTime = 0.4f,
										   .defenderDamage = 50,
										   .baseWallDamage = 10,
										   .idleAnimation = {sprite("b1_alien_walk"), 0.1f},
										   .moveAnimation = {sprite("b1_alien_walk"), 0.1f},
										   .attackAnimation = {sprite("b1_alien_attack"), 0.1f},
										   .dyingAnimation = {sprite("b1_alien_death"), 0.1f, 1}});

	m_enemyTypeRegistry.registerEnemyType({.type = EnemyType::B2,
										   .spawnChance = 0.3f,
										   .maxHp = 150,
										   .speed = 80,
										   .attackTime = 0.5f,
										   .defenderDamage = 50,
										   .baseWallDamage = 10,
										   .idleAnimation = {sprite("b2_alien_walk"), 0.1f},
										   .moveAnimation = {sprite("b2_alien_walk"), 0.1f},
										   .attackAnimation = {sprite("b2_alien_attack"), 0.1f},
										   .dyingAnimation = {sprite("b2_alien_death"), 0.1f, 1}});

	m_enemyTypeRegistry.registerEnemyType({.type = EnemyType::Portal,
										   .spawnChance = 0.5f,
										   .maxHp = 60,
										   .speed = 30,
										   .attackTime = 0.5f,
										   .defenderDamage = 50,
										   .baseWallDamage = 10,
										   .idleAnimation = {sprite("portal_alien_walk"), 0.1f},
										   .moveAnimation = {sprite("portal_alien_walk"), 0.1f},
										   .attackAnimation = {sprite("portal_alien_attack"), 0.1f},
										   .dyingAnimation = {sprite("portal_alien_death"), 0.1f, 1}});
}
