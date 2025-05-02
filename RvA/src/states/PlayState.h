#pragma once
#include "IGameState.h"
#include "Session.h"
#include "GUI/HUD.h"

class PlayState : public IGameState
{
public:
	PlayState(Game& game);
	~PlayState();

	void update(Game& game, float dt) override;
	void draw(Game& game) override;

	void onEnter(Game& game) override;
	void onExit(Game& game) override;

private:
	void goToWinState(Game& game);
	void setupHUD();
	void togglePause();

	int m_numberOfEnemiesToKill{2000};

	bool m_isGamePaused{};

	Game& m_game;
	HUD m_hud;	
	CollisionSystem m_collisionSystem;
	Session m_session;
};