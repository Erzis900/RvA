#include "Game.h"
#include "states/MenuState.h"
#include "constants.h"

Game::Game()
	:m_renderRec(), m_fadingOut(false), m_fadingIn(false), m_fadeAlpha(0.f),
	m_scale(1.f), m_texWidth(TEX_WIDTH), m_texHeight(TEX_HEIGHT),
	m_screenWidth(SCREEN_WIDTH), m_screenHeight(SCREEN_HEIGHT),
	m_transitionSpeed(4.f), m_gui(*this)
{
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(int(m_screenWidth), int(m_screenHeight), "RvA");
	SetTargetFPS(60);

	m_renderTexture = LoadRenderTexture(int(m_texWidth), int(m_texHeight));
	SetTextureFilter(m_renderTexture.texture, TEXTURE_FILTER_POINT);

	m_currentState = std::make_unique<MenuState>();
	m_atlas.load("assets/atlas.png");

	DisableCursor();
}

Game::~Game()
{
	UnloadRenderTexture(m_renderTexture);
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
}

void Game::updateRenderRec()
{
	m_screenWidth = float(GetScreenWidth());
	m_screenHeight = float(GetScreenHeight());

	m_scale = std::min(m_screenWidth / m_texWidth, m_screenHeight / m_texHeight);
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

	DrawTexturePro(m_renderTexture.texture, { 0.f, 0.f, m_texWidth, -m_texHeight }, m_renderRec, { 0.f, 0.f }, 0.f, WHITE);
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
	m_nextState = std::move(newState);
	m_fadingOut = true;
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