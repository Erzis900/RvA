#pragma once
#include "defender/DefenderTypes.h"
#include "atlas/Atlas.h"
#include <raylib.h>

class Game;

class GUI
{
public:
	GUI(Game& game);

	void loadResources();

	void drawGame(float batteryCharge, float scraps);
	void drawCursor();
	void drawHp(float hp, float maxHp, Vector2 pos);

	DefenderType getSelectedDefender() const { return m_selectedDefender; }
	bool isPaused() const { return m_paused; }

	void reset();
private:
	void drawBatteryCharge(float batteryCharge);
	void drawDefenders();
	void drawCosts();

	bool m_paused{};
	bool m_defenderHover{};

	Game& m_game;
	DefenderType m_selectedDefender;

	const SpriteInfo* m_mouseCursorPoint{};
	const SpriteInfo* m_mouseCursorHover{};
};