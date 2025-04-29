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

	int m_texWidth;
	int m_texHeight;

	int m_screenWidth;
	int m_screenHeight;

	float m_fadeAlpha;
	float m_transitionSpeed;
	bool m_fadingOut;
	bool m_fadingIn;

	GUI m_gui;
	Atlas m_atlas;
};