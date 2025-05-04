#pragma once

#include "IGameState.h"

#include "utilities/CallbackRegistry.h"

class Session;
class HUD;

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
	void goToWinState();
	void goToPauseState();

	int m_numberOfEnemiesToKill{2000};
	bool m_isGamePaused{};

	Game& m_game;
	Session& m_session;
};