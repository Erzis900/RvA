#include "Game.h"

#include "Config.h"
#include "GUI/GUI.h"
#include "GameRegistry.h"
#include "LevelDefinition.h"
#include "MusicManager.h"
#include "ResourceSystem.h"
#include "Session.h"
#include "atlas/Atlas.h"
#include "constants.h"
#include "fsm/FsmBuilder.h"
#include "states/EndScreenState.h"
#include "states/IntroState.h"
#include "states/LostState.h"
#include "states/MenuState.h"
#include "states/OptionsState.h"
#include "states/PauseState.h"
#include "states/PlayState.h"
#include "states/ProceedState.h"
#include "states/WinState.h"
#include "utilities/Random.h"

using namespace std::string_literals;

void MyTraceLog(int logLevel, const char* text, va_list args) {
	vfprintf(stdout, text, args);
	fprintf(stdout, "\n");
}

// We do this to avoid recompilation of every class depending on Game.h
struct Game::pimpl {
	pimpl()
		: m_scale(1.f)
		, m_screenWidth(SCREEN_WIDTH)
		, m_screenHeight(SCREEN_HEIGHT)
		, m_gui(m_atlas, m_musicManager)
		, m_gameSession(m_gui, m_resourceSystem, m_gameRegistry, m_config, m_musicManager)
		, m_musicManager(m_config, m_resourceSystem) {
		Random::setInstance(&m_random);
		SetTraceLogCallback(MyTraceLog); // Use our logger
		SetConfigFlags(FLAG_WINDOW_RESIZABLE);
		SetTraceLogLevel(LOG_ALL);
		InitWindow(m_screenWidth, m_screenHeight, "RvA");
		SetTargetFPS(60);

		m_gameRenderTexture = LoadRenderTexture(static_cast<int>(GAME_RENDERTEXTURE_SIZE.x), static_cast<int>(GAME_RENDERTEXTURE_SIZE.y));
		m_uiRenderTexture = LoadRenderTexture(static_cast<int>(UI_RENDERTEXTURE_SIZE.x), static_cast<int>(UI_RENDERTEXTURE_SIZE.y));
		SetTextureFilter(m_gameRenderTexture.texture, TEXTURE_FILTER_POINT);
		SetTextureFilter(m_uiRenderTexture.texture, TEXTURE_FILTER_POINT);

		InitAudioDevice();

		m_resourceSystem.loadResources();

		m_atlas.load("assets/atlas.png");
		m_gui.loadResources();

		DisableCursor();
		m_gui.setCursor(CursorType::Default);

		SetExitKey(0);
	}

	~pimpl() {
		UnloadRenderTexture(m_gameRenderTexture);
		UnloadRenderTexture(m_uiRenderTexture);
		CloseAudioDevice();
		CloseWindow();
	}

	void update() {
		float dt = GetFrameTime();
		updateMouse();

		m_fsm->update(dt);

		if (DEV_MODE && IsKeyPressed(KEY_F2)) {
			m_gui.toggleDebugView();
		}

		m_gui.update(dt);
		m_musicManager.updateStream();
	}

	Rectangle updateRenderRec(Texture& texture) {
		m_screenWidth = GetScreenWidth();
		m_screenHeight = GetScreenHeight();

		m_scale = std::min(float(m_screenWidth) / float(texture.width), float(m_screenHeight) / float(texture.height));
		float width = texture.width * m_scale;
		float height = texture.height * m_scale;
		return {(m_screenWidth - width) / 2.f, (m_screenHeight - height) / 2.f, width, height};
	}

	void updateMouse() {
		int offsetX = int(-(m_screenWidth - (m_gameRenderTexture.texture.width * m_scale)) / 2.f);
		int offsetY = int(-(m_screenHeight - (m_gameRenderTexture.texture.height * m_scale)) / 2.f);

		SetMouseOffset(offsetX, offsetY);
		SetMouseScale(1 / m_scale, 1 / m_scale);
	}

	void draw() {
		auto gameRenderRectangle = updateRenderRec(m_gameRenderTexture.texture);
		// auto uiRenderRectangle = updateRenderRec(m_uiRenderTexture.texture);

		// --- Render Game ---
		BeginTextureMode(m_gameRenderTexture);
		ClearBackground(BLACK);
		m_gameSession.draw(m_atlas);
		m_drawCallbacks.executeCallbacks();
		m_gui.draw();
		EndTextureMode();

		// --- Render UI ---
		/*
		BeginTextureMode(m_uiRenderTexture);
		ClearBackground({0, 0, 0, 0});
		m_gui.draw();
		EndTextureMode();
		*/

		// --- Final Composition ---
		BeginDrawing();

		ClearBackground(BLACK);

		DrawTexturePro(m_gameRenderTexture.texture, {0.f, 0.f, float(m_gameRenderTexture.texture.width), -float(m_gameRenderTexture.texture.height)}, gameRenderRectangle, {0.f, 0.f}, 0.f, WHITE);
		// DrawTexturePro(m_uiRenderTexture.texture, {0.f, 0.f, float(m_uiRenderTexture.texture.width), -float(m_uiRenderTexture.texture.height)}, uiRenderRectangle, {0.f, 0.f}, 0.f, WHITE);

		EndDrawing();
	}

	bool shouldClose() const {
		return WindowShouldClose() || m_fsm->hasReachedExitState();
	}

	RenderTexture2D m_gameRenderTexture;
	RenderTexture2D m_uiRenderTexture;

	float m_scale{};

	int m_screenWidth{};
	int m_screenHeight{};

	bool m_isTransitionInProgress{};

	GUI m_gui;
	Atlas m_atlas;
	Config m_config;
	ResourceSystem m_resourceSystem;
	MusicManager m_musicManager;

	GameRegistry m_gameRegistry;

	Session m_gameSession;
	std::unique_ptr<flow::Fsm> m_fsm;
	CallbackRegistry<> m_drawCallbacks;
	Random m_random;
};

Game::Game() {
	m_pimpl = std::make_unique<pimpl>();

	registerDefenderTypes();
	registerBulletTypes();
	registerEnemyTypes();
	registerDropTypes();
	LevelDefinition::registerLevels(m_pimpl->m_gameRegistry, m_pimpl->m_atlas);
	registerPortals();
	m_pimpl->m_gameRegistry.verifyData();

	setupFSM();
}

Game::~Game() = default;

void Game::run() {
	while (!m_pimpl->shouldClose()) {
		m_pimpl->update();
		m_pimpl->draw();
	}
}

void Game::setupFSM() {
	auto fsmBuilder = flow::FsmBuilder();

	auto& session = getGameSession();

	// clang-format off
	fsmBuilder
		// Intro
		.state<IntroState>("Intro", *this)
			.on("menu").jumpTo("StartToMainMenu")

		// Menus
		.state<ProceedState>("StartToMainMenu", [this, &session](){ 
				session.setDemoMode(true);
				session.restartSession();
				save();
			})
			.on("proceed").jumpTo("MainMenu")

		.state<MenuState>("MainMenu", *this)
			.on("play").jumpTo("StartSession")
			.on("options").jumpTo("Options")
			.on("exit").exitFsm()

		.state<OptionsState>("Options", *this, 0.25f, true, true, true)
			.on("back").jumpTo("MainMenu")
		
		// In Game States
		.state<ProceedState>("StartSession", [this, &session](){ save(); session.restartSession(); })
			.on("proceed").jumpTo("Play")

		.state<ProceedState>("StartLevel", [this, &session](){ session.startNextLevel(); save(); })
			.on("proceed").jumpTo("Play")

		.state<ProceedState>("ResumeLevel", [this, &session](){ session.resume(); })
			.on("proceed").jumpTo("Play")

		.state<PlayState>("Play", *this)
			.on("pause").jumpTo("PauseLevel")
			.on("win").jumpTo("WinScreen")
			.on("end").jumpTo("EndScreen")
			.on("lost").jumpTo("LoseScreen")

		.state<ProceedState>("PauseLevel", [this, &session](){ session.pause(); })
			.on("proceed").jumpTo("PauseMenu")

		.state<PauseState>("PauseMenu", *this)
			.on("resume").jumpTo("ResumeLevel")
			.on("restart").jumpTo("StartSession")
			.on("options").jumpTo("InGameOptions")
			.on("menu").jumpTo("StartToMainMenu")
		
		.state<OptionsState>("InGameOptions", *this, 0.7f, false, false, false)
			.on("back").jumpTo("PauseMenu")

		.state<WinState>("WinScreen", *this)
			.on("next").jumpTo("StartLevel")

		.state<EndScreenState>("EndScreen", *this)
			.on("menu").jumpTo("StartToMainMenu")

		.state<LostState>("LoseScreen", *this)
			.on("restart").jumpTo("StartSession")
			.on("menu").jumpTo("StartToMainMenu");
	// clang-format on

	auto [fsm, fsmInfo] = fsmBuilder.build("Intro", nullptr);
	m_pimpl->m_fsm = std::move(fsm);
}

void Game::save() {
	m_pimpl->m_config.save();
}

void Game::registerDefenderTypes() {
	auto sprite = [this](const char* spriteName) { return m_pimpl->m_atlas.getSpriteInfo(spriteName); };

	m_pimpl->m_gameRegistry.addDefender("Solarpanel",
										{
											.type = DefenderType::Solar,
											.name = "Solar Panel",
											.spriteEnabled = {sprite("solar_idle"), 0.1f},
											.spriteDisabled = {sprite("solar_off"), 0.1f},
											.spriteDying = {sprite("solar_death"), 0.1f, 1},
											.batteryDrain = -5,
											//.scrapsGain = 5,
											.maxHP = 100,
											.buildCooldown = 2.f,
										});

	m_pimpl->m_gameRegistry.addDefender("Shooter",
										{.type = DefenderType::Shooter,
										 .name = "Shooter",
										 .spriteEnabled = {sprite("shooter_idle"), 0.1f},
										 .spriteDisabled = {sprite("shooter_off"), 0.1f},
										 .spriteShoot = {sprite("shooter_shoot"), 0.1f, 1},
										 .spriteDying = {sprite("shooter_death"), 0.1f, 1},
										 .batteryDrain = 5.f,
										 .firstShootCooldown = 1.5f,
										 .shootCooldown = 0.1f,
										 .maxHP = 150,
										 .cost = 10,
										 .bulletType = "SimpleShot",
										 .shootingAnimationTime = 0.6f,
										 .buildCooldown = 3.f});

	m_pimpl->m_gameRegistry.addDefender("Catapult",
										{.type = DefenderType::Catapult,
										 .name = "Catapult",
										 .spriteEnabled = {sprite("catapult_idle"), 0.1f},
										 .spriteDisabled = {sprite("catapult_off"), 0.1f},
										 .spriteShoot = {sprite("catapult_shoot"), 0.1f, 1},
										 .spriteDying = {sprite("catapult_death"), 0.1f, 1},
										 .batteryDrain = 10.f,
										 .firstShootCooldown = 3.f,
										 .shootCooldown = 1.f,
										 .maxHP = 200,
										 .cost = 20,
										 .bulletType = "ChasingShot",
										 .shootingAnimationTime = 0.6f,
										 .buildCooldown = 5.f});

	m_pimpl->m_gameRegistry.addDefender("Lasertron",
										{.type = DefenderType::Lasertron,
										 .name = "Lasertron",
										 .spriteEnabled = {sprite("lasertron_idle"), 0.1f},
										 .spriteDisabled = {sprite("lasertron_off"), 0.1f},
										 .spriteShoot = {sprite("lasertron_shoot"), 0.1f, 1},
										 .spriteDying = {sprite("lasertron_death"), 0.1f, 1},
										 .batteryDrain = 20.f,
										 .firstShootCooldown = 3.f,
										 .shootCooldown = 2.f,
										 .maxHP = 250,
										 .cost = 30,
										 .bulletType = "LaserBeam",
										 .shootingAnimationTime = 1.0f,
										 .buildCooldown = 5.f});

	/*
	 * Defenders Tweaked for tutorials
	 */
	m_pimpl->m_gameRegistry.addDefender("SolarpanelTutorial",
										{
											.type = DefenderType::Solar,
											.name = "Solar Panel",
											.spriteEnabled = {sprite("solar_idle"), 0.1f},
											.spriteDisabled = {sprite("solar_off"), 0.1f},
											.spriteDying = {sprite("solar_death"), 0.1f, 1},
											.batteryDrain = -15,
											.maxHP = 100,
											.buildCooldown = 2.f,
										});

	m_pimpl->m_gameRegistry.addDefender("ShooterTutorial",
										{.type = DefenderType::Shooter,
										 .name = "Shooter",
										 .spriteEnabled = {sprite("shooter_idle"), 0.1f},
										 .spriteDisabled = {sprite("shooter_off"), 0.1f},
										 .spriteShoot = {sprite("shooter_shoot"), 0.1f, 1},
										 .spriteDying = {sprite("shooter_death"), 0.1f, 1},
										 .batteryDrain = 5.f,
										 .firstShootCooldown = 1.5f,
										 .shootCooldown = 0.2f,
										 .maxHP = 150,
										 .cost = 20,
										 .bulletType = "SimpleShot",
										 .shootingAnimationTime = 0.6f,
										 .buildCooldown = 3.f});
}

void Game::registerBulletTypes() {
	m_pimpl->m_gameRegistry.addBullet("SimpleShot",
									  BulletShotData{
										  .velocity = {150, 0},
										  .offsetPos = {24, 7},
										  .radius = 2.f,
										  .damage = {25, 16},
										  .maxLifetime = 3.2f,
									  });

	m_pimpl->m_gameRegistry.addBullet("ChasingShot",
									  ChasingShotData{
										  .startOffset = {20, 8},
										  .radius = 10.f,
										  .damage = {50, 16},
										  .maxLifetime = 5.f,
										  .speed = 150,
										  .color = {165, 48, 48, 255},
										  .direction = {1, 0},
									  });

	m_pimpl->m_gameRegistry.addBullet("LaserBeam",
									  LaserBeamData{
										  .startOffset = {30, 15},
										  .beamHeight = 4,
										  .damage = {100.f, 0, true},
										  .auraSize = 2,
										  .beamStartColor = {245, 125, 74, 255},
										  .beamEndColor = RED,
										  .maxLifetime = 0.4f,
										  .shootAnimationSpeed = 15,
										  .shootAnimationDuration = 2.f,
									  });
}

void Game::registerEnemyTypes() {
	auto sprite = [this](const char* spriteName) { return m_pimpl->m_atlas.getSpriteInfo(spriteName); };

	m_pimpl->m_gameRegistry.addEnemy("B1",
									 {.type = EnemyType::B1,
									  .maxHp = 100,
									  .speed = 40,
									  .attackTime = 0.4f,
									  .defenderDamage = 50,
									  .baseWallDamage = 10,
									  .dropType = "simpleScraps",
									  .dropAmount = FixedValue{10},
									  .idleAnimation = {sprite("b1_alien_walk"), 0.1f},
									  .moveAnimation = {sprite("b1_alien_walk"), 0.1f},
									  .attackAnimation = {sprite("b1_alien_attack"), 0.1f},
									  .dyingAnimation = {sprite("b1_alien_death"), 0.1f, 1},
									  .sparkEffect = {sprite("spark"), 0.1f}});

	m_pimpl->m_gameRegistry.addEnemy("B2",
									 {.type = EnemyType::B2,
									  .maxHp = 150,
									  .speed = 80,
									  .attackTime = 0.5f,
									  .defenderDamage = 50,
									  .baseWallDamage = 10,
									  .dropType = "simpleScraps",
									  .dropAmount = RandomRangeStep{20, 40, 10},
									  .idleAnimation = {sprite("b2_alien_walk"), 0.1f},
									  .moveAnimation = {sprite("b2_alien_walk"), 0.1f},
									  .attackAnimation = {sprite("b2_alien_attack"), 0.1f},
									  .dyingAnimation = {sprite("b2_alien_death"), 0.1f, 1},
									  .sparkEffect = {sprite("spark"), 0.1f, 1}});

	m_pimpl->m_gameRegistry.addEnemy("Portal",
									 {.type = EnemyType::Portal,
									  .maxHp = 60,
									  .speed = 30,
									  .attackTime = 0.5f,
									  .defenderDamage = 50,
									  .baseWallDamage = 10,
									  .dropType = "simpleScraps",
									  .dropAmount = RandomRangeStep{100, 200, 10},
									  .idleAnimation = {sprite("portal_alien_walk"), 0.1f},
									  .moveAnimation = {sprite("portal_alien_walk"), 0.1f},
									  .attackAnimation = {sprite("portal_alien_attack"), 0.1f},
									  .dyingAnimation = {sprite("portal_alien_death"), 0.1f, 1},
									  .summonAnimation = {sprite("portal_alien_summon"), 0.1f, 1},
									  .sparkEffect = {sprite("spark"), 0.1f, 1},
									  .behavior = PortalSpawnBehaviorInfo{
										  .timeBeforeActing = 2.5f,
										  .timeBeforeActingAgain = 1.f,
										  .chanceIncreaseOverTime = 1.f,
										  .baseChanceToSpawn = 30.f,
										  .columnDistance = RandomRange{4, 8},
										  .rowDistance = RandomRange{-2, 2},
										  .portalCastRange = FixedValue{3},
										  .animation = {sprite("portal_alien_summon"), 0.1f, 1},
									  }});

	/*
	 * Enemies Tweaked for tutorials
	 */
	m_pimpl->m_gameRegistry.addEnemy("B1Tutorial",
									 {.type = EnemyType::B1,
									  .maxHp = 100,
									  .speed = 40,
									  .attackTime = 0.4f,
									  .defenderDamage = 50,
									  .baseWallDamage = 10,
									  .dropType = "simpleScraps",
									  .dropAmount = FixedValue{10},
									  .idleAnimation = {sprite("b1_alien_walk"), 0.1f},
									  .moveAnimation = {sprite("b1_alien_walk"), 0.1f},
									  .attackAnimation = {sprite("b1_alien_attack"), 0.1f},
									  .dyingAnimation = {sprite("b1_alien_death"), 0.1f, 1},
									  .sparkEffect = {sprite("spark"), 0.1f, 1}});

	m_pimpl->m_gameRegistry.addEnemy("B1TutorialTank",
									 {.type = EnemyType::B1,
									  .maxHp = 450,
									  .speed = 40,
									  .attackTime = 0.4f,
									  .defenderDamage = 50,
									  .baseWallDamage = 10,
									  .dropType = "simpleScraps",
									  .dropAmount = FixedValue{10},
									  .idleAnimation = {sprite("b1_alien_walk"), 0.1f},
									  .moveAnimation = {sprite("b1_alien_walk"), 0.1f},
									  .attackAnimation = {sprite("b1_alien_attack"), 0.1f},
									  .dyingAnimation = {sprite("b1_alien_death"), 0.1f, 1},
									  .sparkEffect = {sprite("spark"), 0.1f, 1}});

	m_pimpl->m_gameRegistry.addEnemy("B1TutorialTank2",
									 {.type = EnemyType::B1,
									  .maxHp = 300,
									  .speed = 50,
									  .attackTime = 0.4f,
									  .defenderDamage = 50,
									  .baseWallDamage = 10,
									  .dropType = "simpleScraps",
									  .dropAmount = FixedValue{10},
									  .idleAnimation = {sprite("b1_alien_walk"), 0.1f},
									  .moveAnimation = {sprite("b1_alien_walk"), 0.1f},
									  .attackAnimation = {sprite("b1_alien_attack"), 0.1f},
									  .dyingAnimation = {sprite("b1_alien_death"), 0.1f, 1},
									  .sparkEffect = {sprite("spark"), 0.1f, 1}});

	m_pimpl->m_gameRegistry.addEnemy("B1TutorialFast",
									 {.type = EnemyType::B1,
									  .maxHp = 450,
									  .speed = 80,
									  .attackTime = 0.4f,
									  .defenderDamage = 50,
									  .baseWallDamage = 10,
									  .dropType = "simpleScraps",
									  .dropAmount = FixedValue{10},
									  .idleAnimation = {sprite("b1_alien_walk"), 0.1f},
									  .moveAnimation = {sprite("b1_alien_walk"), 0.1f},
									  .attackAnimation = {sprite("b1_alien_attack"), 0.1f},
									  .dyingAnimation = {sprite("b1_alien_death"), 0.1f, 1},
									  .sparkEffect = {sprite("spark"), 0.1f, 1}});
}

void Game::registerDropTypes() {
	auto sprite = [this](const char* spriteName) { return m_pimpl->m_atlas.getSpriteInfo(spriteName); };

	m_pimpl->m_gameRegistry.addDrop("simpleScraps", {.type = DropType::Scraps, .idleAnimation = {sprite("scraps_icon"), 0.1f}, .animationDuration = 0.5f, .size = Vector2{24, 24}});
}

void Game::registerPortals() {
	auto sprite = [this](const char* spriteName) { return m_pimpl->m_atlas.getSpriteInfo(spriteName); };

	m_pimpl->m_gameRegistry.addPortal("entrance",
									  {.type = PortalType::Entrance,
									   .spriteIdle = {sprite("entrance_portal_idle"), 0.1f},
									   .spriteOpen = {sprite("entrance_portal_open"), 0.25f, 1},
									   .spriteClose = {sprite("entrance_portal_open"), 0.25f, 1, true},
									   .lifespan = 4.f});

	m_pimpl->m_gameRegistry.addPortal("exit",
									  {.type = PortalType::Exit,
									   .spriteIdle = {sprite("exit_portal_idle"), 0.1f},
									   .spriteOpen = {sprite("exit_portal_open"), 0.25f, 1},
									   .spriteClose = {sprite("exit_portal_open"), 0.25f, 1, true},
									   .lifespan = 4.f});
}

GUI& Game::getGUI() {
	return m_pimpl->m_gui;
}

Atlas& Game::getAtlas() {
	return m_pimpl->m_atlas;
}

const GameRegistry& Game::getGameRegistry() const {
	return m_pimpl->m_gameRegistry;
}

MusicManager& Game::getMusicManager() {
	return m_pimpl->m_musicManager;
}

Session& Game::getGameSession() {
	return m_pimpl->m_gameSession;
}

Config& Game::getConfig() {
	return m_pimpl->m_config;
}

ResourceSystem& Game::getResourceSystem() {
	return m_pimpl->m_resourceSystem;
}

// When it comes to rendering we should move to a more retained approach
// Instead of propagating calls by calling Class::draw the idea is to move out from that pattern
// and as a temporary solution we can use a callback system instead.
// If someone is interested in rendering something custom they can register a callback
[[nodiscard]] CallbackHandle Game::registerDrawingCallback(std::function<void()> callback) {
	return m_pimpl->m_drawCallbacks.registerCallback(std::move(callback));
}
