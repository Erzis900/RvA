#include "Game.h"
#include "states/MenuState.h"
#include "constants.h"

Game::Game()
	:m_renderRec(), m_fadingOut(false), m_fadingIn(false), m_fadeAlpha(0.f),
	m_scale(1.f), m_texWidth(TEX_WIDTH), m_texHeight(TEX_HEIGHT),
	m_screenWidth(SCREEN_WIDTH), m_screenHeight(SCREEN_HEIGHT),
	m_transitionSpeed(4.f), m_gui(*this), m_guiHelper(*this)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(m_screenWidth, m_screenHeight, "RvA");
	SetTargetFPS(60);

	m_renderTexture = LoadRenderTexture(m_texWidth, m_texHeight);
	SetTextureFilter(m_renderTexture.texture, TEXTURE_FILTER_POINT);

	m_atlas.load("assets/atlas.png");

	DisableCursor();

	registerDefenderTypes();
	registerBulletTypes();

	InitAudioDevice();
	m_musicManager.load();

	m_currentState = std::make_unique<MenuState>();
	m_currentState->onEnter(*this);
}

Game::~Game()
{
	UnloadRenderTexture(m_renderTexture);
	CloseAudioDevice();
	CloseWindow();
}

void Game::update()
{
	float dt = GetFrameTime();
	updateRenderRec();
	updateMouse();

	if (m_fadingOut || m_fadingIn)
	{
		updateTransition(dt);
	}
	else
	{
		m_currentState->update(*this, dt);
	}

	m_musicManager.updateStream();
}

void Game::updateRenderRec()
{
	m_screenWidth = GetScreenWidth();
	m_screenHeight = GetScreenHeight();

	m_scale = std::min(float(m_screenWidth) / float(m_texWidth), float(m_screenHeight) / float(m_texHeight));
	m_renderRec.width = m_texWidth * m_scale;
	m_renderRec.height = m_texHeight * m_scale;
	m_renderRec.x = (m_screenWidth - m_renderRec.width) / 2.f;
	m_renderRec.y = (m_screenHeight - m_renderRec.height) / 2.f;
}

void Game::updateMouse()
{
	int offsetX = int(-(m_screenWidth - (m_texWidth * m_scale)) / 2.f);
	int offsetY = int(-(m_screenHeight - (m_texHeight * m_scale)) / 2.f);

	SetMouseOffset(offsetX, offsetY);
	SetMouseScale(1 / m_scale, 1 / m_scale);
}

void Game::draw()
{
	BeginTextureMode(m_renderTexture);

	ClearBackground(GRAY);

	m_currentState->draw(*this);

	m_gui.drawCursor();

	if (m_fadingOut || m_fadingIn)
	{
		Color fadeColor = Fade(BLACK, m_fadeAlpha);
		DrawRectangle(0, 0, int(m_texWidth), int(m_texHeight), fadeColor);
	}

	drawFPS();
	EndTextureMode();

	BeginDrawing();
	ClearBackground(BLACK);

	DrawTexturePro(m_renderTexture.texture, { 0.f, 0.f, float(m_texWidth), -float(m_texHeight) }, m_renderRec, { 0.f, 0.f }, 0.f, WHITE);
	EndDrawing();
}

void Game::drawFPS()
{
	const char* fpsText = TextFormat("%d", GetFPS());
	int textWidth = MeasureText(fpsText, 10);

	DrawText(fpsText, int(m_texWidth) - 16 - textWidth / 2, int(m_texHeight) - 20, 10, GREEN);
}

void Game::setState(std::unique_ptr<IGameState> newState)
{
	m_currentState->onExit(*this);

	m_nextState = std::move(newState);
	m_fadingOut = true;

	m_nextState->onEnter(*this);
}

void Game::updateTransition(float dt)
{
	if (m_fadingOut)
	{
		m_fadeAlpha += dt * m_transitionSpeed;
		if (m_fadeAlpha >= 1.f)
		{
			m_currentState = std::move(m_nextState);
			m_fadingOut = false;
			m_fadingIn = true;
		}
	}
	else if (m_fadingIn)
	{
		m_fadeAlpha -= dt * m_transitionSpeed;
		if (m_fadeAlpha <= 0.f)
		{
			m_fadingIn = false;
			m_fadeAlpha = 0.f;
		}
	}
}

void Game::run()
{
	while (!WindowShouldClose())
	{
		update();
		draw();
	}
}

void Game::registerDefenderTypes()
{
	m_defenderTypeRegistry.registerDefender({
		.type = DefenderType::Solar,
		.spriteEnabled = "solar_idle",
		.spriteDisabled = "solar_off",
		.batteryDrain = -5,
		.scrapsGain = 5,
		.maxHP = 100,
	});

	m_defenderTypeRegistry.registerDefender({
		.type = DefenderType::Shooter,
		.spriteEnabled = "shooter_idle",
		.spriteDisabled = "shooter_off",
		.batteryDrain = 5.f,
		.firstShootCooldown = 1.f,
		.shootCooldown = 1.f,
		.maxHP = 150,
		.cost = 10,
		.bulletType = "SimpleShot",
	});

	m_defenderTypeRegistry.registerDefender({
		.type = DefenderType::Catapult,
		.spriteEnabled = "catapult_idle",
		.spriteDisabled = "catapult_off",
		.batteryDrain = 10.f,
		.firstShootCooldown = 1.f,
		.shootCooldown = 1.f,
		.maxHP = 200,
		.cost = 20,
		.bulletType = "ChasingShot"
	});

	m_defenderTypeRegistry.registerDefender({
		.type = DefenderType::Lasertron,
		.spriteEnabled = "lasertron_idle",
		.spriteDisabled = "lasertron_off",
		.batteryDrain = 20.f,
		.firstShootCooldown = 0.5f,
		.shootCooldown = 4.f,
		.maxHP = 250,
		.cost = 30,
		.bulletType = "LaserBeam"
	});
}

void Game::registerBulletTypes()
{
	m_bulletTypeRegistry.registerBulletType("SimpleShot", BulletShotData {
		.velocity = { 150, 0 },
		.radius = 5.f,
		.damage = 50,
		.maxLifetime = 100,
	});

	m_bulletTypeRegistry.registerBulletType("ChasingShot", ChasingShotData {
		.startOffset = { 20, 20 },
		.radius = 5.f,
		.damage = 50,
		.maxLifetime = 100,
		.speed = 150,
		.direction = { 1, 0 },
	});

	m_bulletTypeRegistry.registerBulletType("LaserBeam", LaserBeamData{
		.startOffset = { 35, 18 },
		.beamHeight = 6,
		.damage = 2.5f,
		.auraSize = 2,
		.beamColor = BLUE,
		.auraColor = {255, 255, 255, 200},
		.maxLifetime = 2.f,
		.shootAnimationSpeed = 15,
		.shootAnimationDuration = 2.f
	});
}