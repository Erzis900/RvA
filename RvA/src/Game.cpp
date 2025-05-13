#include "Game.h"

#include "Config.h"
#include "GUI/GUI.h"
#include "GameRegistry.h"
#include "MusicManager.h"
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
#include "states/WinState.h"
#include "utilities/Random.h"

using namespace std::string_literals;

// We do this to avoid recompilation of every class depending on Game.h
struct Game::pimpl {
	pimpl() : m_scale(1.f), m_screenWidth(SCREEN_WIDTH), m_screenHeight(SCREEN_HEIGHT), m_gui(m_atlas, m_musicManager), m_gameSession(m_gui, m_gameRegistry), m_musicManager(m_config) {
		Random::setInstance(&m_random);
		SetConfigFlags(FLAG_WINDOW_RESIZABLE);
		InitWindow(m_screenWidth, m_screenHeight, "RvA");
		SetTargetFPS(60);

		m_gameRenderTexture = LoadRenderTexture(static_cast<int>(GAME_RENDERTEXTURE_SIZE.x), static_cast<int>(GAME_RENDERTEXTURE_SIZE.y));
		m_uiRenderTexture = LoadRenderTexture(static_cast<int>(UI_RENDERTEXTURE_SIZE.x), static_cast<int>(UI_RENDERTEXTURE_SIZE.y));
		SetTextureFilter(m_gameRenderTexture.texture, TEXTURE_FILTER_POINT);
		SetTextureFilter(m_uiRenderTexture.texture, TEXTURE_FILTER_POINT);

		m_atlas.load("assets/atlas.png");
		m_gui.loadResources();

		DisableCursor();
		m_gui.setCursor(CursorType::Default);

		InitAudioDevice();
		m_musicManager.load();
		m_gui.setDefaultButtonSound(&m_musicManager.getButtonClick());

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
	registerLevels();
	registerPortals();
	verifyLevelData();

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

	// clang-format off
	fsmBuilder
		.state<IntroState>("Intro", *this)
			.on("menu").jumpTo("MainMenu")

		// Menus
		.state<MenuState>("MainMenu", *this)
			.on("play").jumpTo("Play")
			.on("options").jumpTo("Options")
			.on("exit").exitFsm()

		.state<OptionsState>("Options", *this, 0.25f, true, true, true)
			.on("back").jumpTo("MainMenu")

		// In Game States
		.state<PlayState>("Play", *this)
			.on("pause").jumpTo("PauseMenu")
			.on("win").jumpTo("WinScreen")
			.on("end").jumpTo("EndScreen")
			.on("lost").jumpTo("LoseScreen")

		.state<PauseState>("PauseMenu", *this)
			.on("resume").jumpTo("Play")
			.on("restart").jumpTo("Play")
			.on("options").jumpTo("InGameOptions")
			.on("menu").jumpTo("MainMenu")
		
		.state<OptionsState>("InGameOptions", *this, 0.7f, false, false, false)
			.on("back").jumpTo("PauseMenu")

		.state<WinState>("WinScreen", *this)
			.on("next").jumpTo("Play")

		.state<EndScreenState>("EndScreen", *this)
			.on("menu").jumpTo("MainMenu")

		.state<LostState>("LoseScreen", *this)
			.on("restart").jumpTo("Play")
			.on("menu").jumpTo("MainMenu");
	// clang-format on

	std::string startState = "Intro";
	if (DEV_MODE) {
		startState = "MainMenu";
	}

	auto [fsm, fsmInfo] = fsmBuilder.build(startState, nullptr);
	m_pimpl->m_fsm = std::move(fsm);
}

void Game::registerDefenderTypes() {
	auto sprite = [this](const char* spriteName) { return m_pimpl->m_atlas.getSpriteInfo(spriteName); };

	m_pimpl->m_gameRegistry.addDefender("Solarpanel",
										{
											.type = DefenderType::Solar,
											.name = "Solar Panel",
											.spriteEnabled = {sprite("solar_idle"), 0.1f},
											.spriteDisabled = {sprite("solar_off"), 0.1f},
											.spriteDying = {sprite("b1_alien_death"), 0.1f, 1}, // TODO add respective dying animation (art not done)
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
										 .spriteDying = {sprite("b1_alien_death"), 0.1f, 1}, // TODO add respective dying animation (art not done)
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
										 .spriteDying = {sprite("b1_alien_death"), 0.1f, 1}, // TODO add respective dying animation (art not done)
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
										 .spriteDying = {sprite("b1_alien_death"), 0.1f, 1},
										 .batteryDrain = 20.f,
										 .firstShootCooldown = 3.f,
										 .shootCooldown = 2.f,
										 .maxHP = 250,
										 .cost = 30,
										 .bulletType = "LaserBeam",
										 .shootingAnimationTime = 1.0f,
										 .buildCooldown = 5.f});
}

void Game::registerBulletTypes() {
	m_pimpl->m_gameRegistry.addBullet("SimpleShot",
									  BulletShotData{
										  .velocity = {150, 0},
										  .offsetPos = {24, 7},
										  .radius = 2.f,
										  .damage = {25, 16},
										  .maxLifetime = 3.f,
									  });

	m_pimpl->m_gameRegistry.addBullet("ChasingShot",
									  ChasingShotData{
										  .startOffset = {20, 8},
										  .radius = 10.f,
										  .damage = {50, 16},
										  .maxLifetime = 3.f,
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
									  .spawnChance = 0.2f,
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
									  .dyingAnimation = {sprite("b1_alien_death"), 0.1f, 1}});

	m_pimpl->m_gameRegistry.addEnemy("B2",
									 {.type = EnemyType::B2,
									  .spawnChance = 0.3f,
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
									  .dyingAnimation = {sprite("b2_alien_death"), 0.1f, 1}});

	m_pimpl->m_gameRegistry.addEnemy("Portal",
									 {.type = EnemyType::Portal,
									  .spawnChance = 0.5f,
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
}

void Game::registerDropTypes() {
	auto sprite = [this](const char* spriteName) { return m_pimpl->m_atlas.getSpriteInfo(spriteName); };

	m_pimpl->m_gameRegistry.addDrop("simpleScraps", {.type = DropType::Scraps, .idleAnimation = {sprite("scraps_icon"), 0.1f}, .animationDuration = 0.5f, .size = Vector2{24, 24}});
}

void Game::registerLevels() {
	auto sprite = [this](const char* spriteName) { return m_pimpl->m_atlas.getSpriteInfo(spriteName); };

	constexpr int MAX_BATTERY_CHARGE = 100;
	std::string b1 = "B1";
	std::string b2 = "B2";
	std::string portal = "Portal";

	auto lastColumn = FixedValue{18};

	auto selection = EntitySelection{{b1, 0.7f}, {b2, 0.2f}, {portal, 200.1f}};
	m_pimpl->m_gameRegistry.addLevel("level1",
									 {
										 .name = "Level 1",
										 .startingScraps = 100,
										 .maxBatteryCharge = MAX_BATTERY_CHARGE,
										 .winCountdownDuration = 2.f,
										 .winCondition = AllWavesGoneCondition{},
										 .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
										 .groundBackground = sprite("ground_bkg"),
										 .topBackground = sprite("top_bkg"),
										 .timeline =
											 {
												 .keyframes =
													 {
														 {0.f,
														  SpawnEntityBurstOperation{
															  .amount = FixedValue{1},
															  .interval = FixedValue{1.f},
															  .row = RandomRange{0, 6},
															  .column = lastColumn,
															  .id = selection,
														  }},
														 {3.f, SpawnEntityOperation{.row = FixedValue{3}, .column = lastColumn, .id = FixedValue{"B1"s}}},
														 {5.f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{"B1"s}}},
														 {7.f, SpawnEntityOperation{.row = FixedValue{6}, .column = lastColumn, .id = FixedValue{"B1"s}}},
														 {15.f,
														  SpawnEntityBurstOperation{
															  .amount = FixedValue{10},
															  .interval = FixedValue{1.f},
															  .row = RandomRange{0, 6},
															  .column = lastColumn,
															  .id = selection,
														  }},
													 },
											 },
									 });

	m_pimpl->m_gameRegistry.addLevel("level2",
									 {
										 .name = "Level 2",
										 .startingScraps = 100,
										 .maxBatteryCharge = MAX_BATTERY_CHARGE,
										 .winCountdownDuration = 2.f,
										 .winCondition = AllWavesGoneCondition{},
										 .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
										 .groundBackground = sprite("ground_bkg"),
										 .topBackground = sprite("top_bkg"),
										 .timeline =
											 {
												 .keyframes =
													 {
														 {2.f,
														  SpawnEntityBurstOperation{
															  .amount = FixedValue{5},
															  .interval = FixedValue{1.f},
															  .row = RandomRange{0, 6},
															  .column = lastColumn,
															  .id = FixedValue{"B1"s},
														  }},
														 {10.f,
														  SpawnEntityBurstOperation{
															  .amount = FixedValue{5},
															  .interval = FixedValue{1.f},
															  .row = RandomRange{0, 6},
															  .column = lastColumn,
															  .id = FixedValue{"B2"s},
														  }},
														 {20.f,
														  SpawnEntityBurstOperation{
															  .amount = FixedValue{5},
															  .interval = FixedValue{1.f},
															  .row = RandomRange{0, 6},
															  .column = lastColumn,
															  .id = FixedValue{"Portal"s},
														  }},
													 },
											 },
									 });

	auto enemyAt = [&](int row) { return SpawnEntityOperation{.row = FixedValue{row}, .column = lastColumn, .id = FixedValue{"B1"s}}; };

	auto space = 2.5f;

	m_pimpl->m_gameRegistry.addLevel("level3",
									 {
										 .name = "Hello",
										 .startingScraps = 100,
										 .maxBatteryCharge = 10000,
										 .winCountdownDuration = 2.f,
										 .winCondition = AllWavesGoneCondition{},
										 .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
										 .groundBackground = sprite("ground_bkg"),
										 .topBackground = sprite("top_bkg"),
										 .timeline = {.keyframes =
														  {
															  // H
															  {space + 0.f, enemyAt(1)},
															  {space + 0.f, enemyAt(2)},
															  {space + 0.f, enemyAt(3)},
															  {space + 0.f, enemyAt(4)},
															  {space + 0.f, enemyAt(5)},
															  {space + 0.5f, enemyAt(3)},
															  {space + 1.f, enemyAt(3)},
															  {space + 1.5f, enemyAt(1)},
															  {space + 1.5f, enemyAt(2)},
															  {space + 1.5f, enemyAt(3)},
															  {space + 1.5f, enemyAt(4)},
															  {space + 1.5f, enemyAt(5)},

															  // E
															  {space * 2 + 0.5f, enemyAt(1)},
															  {space * 2 + 0.5f, enemyAt(2)},
															  {space * 2 + 0.5f, enemyAt(3)},
															  {space * 2 + 0.5f, enemyAt(4)},
															  {space * 2 + 0.5f, enemyAt(5)},
															  {space * 2 + 1.f, enemyAt(1)},
															  {space * 2 + 1.f, enemyAt(3)},
															  {space * 2 + 1.f, enemyAt(5)},
															  {space * 2 + 1.5f, enemyAt(1)},
															  {space * 2 + 1.5f, enemyAt(3)},
															  {space * 2 + 1.5f, enemyAt(5)},

															  // L
															  {space * 3 + 0.5f, enemyAt(1)},
															  {space * 3 + 0.5f, enemyAt(2)},
															  {space * 3 + 0.5f, enemyAt(3)},
															  {space * 3 + 0.5f, enemyAt(4)},
															  {space * 3 + 0.5f, enemyAt(5)},
															  {space * 3 + 1.f, enemyAt(5)},
															  {space * 3 + 1.5f, enemyAt(5)},

															  // L
															  {space * 4 + 0.5f, enemyAt(1)},
															  {space * 4 + 0.5f, enemyAt(2)},
															  {space * 4 + 0.5f, enemyAt(3)},
															  {space * 4 + 0.5f, enemyAt(4)},
															  {space * 4 + 0.5f, enemyAt(5)},
															  {space * 4 + 1.f, enemyAt(5)},
															  {space * 4 + 1.5f, enemyAt(5)},

															  // O
															  {space * 5 + 0.5f, enemyAt(1)},
															  {space * 5 + 0.5f, enemyAt(2)},
															  {space * 5 + 0.5f, enemyAt(3)},
															  {space * 5 + 0.5f, enemyAt(4)},
															  {space * 5 + 0.5f, enemyAt(5)},
															  {space * 5 + 1.f, enemyAt(1)},
															  {space * 5 + 1.f, enemyAt(5)},
															  {space * 5 + 1.5f, enemyAt(1)},
															  {space * 5 + 1.5f, enemyAt(2)},
															  {space * 5 + 1.5f, enemyAt(3)},
															  {space * 5 + 1.5f, enemyAt(4)},
															  {space * 5 + 1.5f, enemyAt(5)},
														  }},
									 });

	auto defenderSelection = EntitySelection{{"Solarpanel", 0.25f}, {"Lasertron", 0.25f}, {"Catapult", 0.25f}, {"Shooter", 0.25f}};
	m_pimpl->m_gameRegistry.addLevel("demoLevel",
									 {.name = "",
									  .startingScraps = 900,
									  .maxBatteryCharge = 1000,
									  .winCountdownDuration = 2.f,
									  .winCondition = AllWavesGoneCondition{},
									  .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
									  .groundBackground = sprite("ground_bkg"),
									  .topBackground = sprite("top_bkg"),
									  .timeline = {.keyframes = {
													   {0.f, SpawnEntityOperation{.row = RandomRange{0, 6}, .column = FixedValue{0}, .id = defenderSelection, .type = EntityType::Defender}},
													   {0.f, SpawnEntityOperation{.row = RandomRange{0, 6}, .column = FixedValue{1}, .id = defenderSelection, .type = EntityType::Defender}},
													   {0.f, SpawnEntityOperation{.row = RandomRange{0, 6}, .column = FixedValue{2}, .id = defenderSelection, .type = EntityType::Defender}},
													   {0.f, SpawnEntityOperation{.row = RandomRange{0, 6}, .column = FixedValue{3}, .id = defenderSelection, .type = EntityType::Defender}},
													   {0,
														SpawnEntityBurstOperation{
															.amount = FixedValue{2000},
															.interval = RandomRange{1.f, 4.f},
															.row = RandomRange{0, 6},
															.column = lastColumn,
															.id = selection,
														}},
												   }}});
}

void Game::verifyLevelData() {
	auto levels = m_pimpl->m_gameRegistry.getLevels();
	for (const auto& [levelName, level] : levels) {
		// verify that the keyframes are defined in the right order
		for (size_t i = 0; i < level.timeline.keyframes.size(); ++i) {
			if (i != 0 && level.timeline.keyframes[i].time < level.timeline.keyframes[i - 1].time) {
				assert(0 && "Keyframes are not in the right order");
			}

			// verify that the SpawnEnemyOperation and SpawnEnemyOperationBurst are indicating valid rows and columns
			if (std::holds_alternative<SpawnEntityOperation>(level.timeline.keyframes[i].action)) {
				auto& spawnEnemyOp = std::get<SpawnEntityOperation>(level.timeline.keyframes[i].action);
				verifyLevelCoordinate(0, ROWS, spawnEnemyOp.row);
				verifyLevelCoordinate(0, COLS + 1, spawnEnemyOp.column);
			} else if (std::holds_alternative<SpawnEntityBurstOperation>(level.timeline.keyframes[i].action)) {
				auto& spawnBurstOp = std::get<SpawnEntityBurstOperation>(level.timeline.keyframes[i].action);
				verifyLevelCoordinate(0, ROWS, spawnBurstOp.row);
				verifyLevelCoordinate(0, COLS + 1, spawnBurstOp.column);
			}
		}
	}
}

void Game::verifyLevelCoordinate(int min, int max, const ConfigValue<int>& value) {
	std::visit(
		[&](auto&& arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, FixedValue<int>>) {
				assert(arg.value >= min && arg.value < max);
			} else if constexpr (std::is_same_v<T, RandomRange<int>>) {
				assert(arg.min >= min && arg.min < arg.max && arg.max >= min && arg.max < max);
			} else if constexpr (std::is_same_v<T, RandomRangeStep<int>>) {
				assert(arg.min >= min && arg.min < arg.max && arg.max >= min && arg.max < max);
			} else if constexpr (std::is_same_v<T, Selection<int>>) {
				for (const auto& weightedValue : arg) {
					assert(weightedValue.value >= min && weightedValue.value < max);
				}
			} else {
				static_assert(sizeof(T) == 0, "Non-exhaustive visitor!");
			}
		},
		value);
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

// When it comes to rendering we should move to a more retained approach
// Instead of propagating calls by calling Class::draw the idea is to move out from that pattern
// and as a temporary solution we can use a callback system instead.
// If someone is interested in rendering something custom they can register a callback
[[nodiscard]] CallbackHandle Game::registerDrawingCallback(std::function<void()> callback) {
	return m_pimpl->m_drawCallbacks.registerCallback(std::move(callback));
}

void Game::registerPortals() {
	auto sprite = [this](const char* spriteName) { return m_pimpl->m_atlas.getSpriteInfo(spriteName); };

	m_pimpl->m_gameRegistry.addPortal("entrance", {.type = PortalType::Entrance, .spriteIdle = {sprite("portal_idle"), 0.1f}, .spriteClose = {sprite("portal_close"), 0.1f, 1, true}, .lifespan = 4.f});

	// Entrance and close the same for now
	m_pimpl->m_gameRegistry.addPortal("exit", {.type = PortalType::Exit, .spriteIdle = {sprite("portal_idle"), 0.1f}, .spriteClose = {sprite("portal_close"), 0.1f, 1, true}, .lifespan = 4.f});
}
