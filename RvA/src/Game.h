#pragma once
#include "IGameState.h"
#include <memory>
#include <raylib.h>
#include "GUI/GUI.h"
#include "atlas/Atlas.h"

class Game
{
public:
	Game();
	~Game();

	void setState(std::unique_ptr<IGameState> newState);
	void run();

	Vector2 getTexSize() { return Vector2(m_texWidth, m_texHeight); }

	GUI& getGUI() { return m_gui; }
	Atlas& getAtlas() { return m_atlas; }
private:
	std::unique_ptr<IGameState> m_currentState;
	std::unique_ptr<IGameState> m_nextState;

	void update();
	void updateRenderRec();
	void updateMouse();
	void updateTransition(float dt);

	void draw();
	void drawFPS();

	RenderTexture2D m_renderTexture;
	Rectangle m_renderRec;

	float m_scale;
	float m_texWidth;
	float m_texHeight;

	float m_screenWidth;
	float m_screenHeight;

	// Transition
	float m_fadeAlpha;
	bool m_fadingOut;
	bool m_fadingIn;
	float m_transitionTime;

	GUI m_gui;
	Atlas m_atlas;
};