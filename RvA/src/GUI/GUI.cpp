#include "GUI.h"
#include "Game.h"
#include "states/MenuState.h"
#include "states/PlayState.h"
#include "rlgl.h"

GUI::GUI(Game& game)
{
	initMenu(game);
	initGame(game);
}

void GUI::initMenu(Game& game)
{
	Vector2 btnSize = { 200.f, 60.f };

	m_playButton = rgc::Button(rgc::Bounds({ game.getTexSize().x / 2 - btnSize.x / 2,
		game.getTexSize().y / 2 - btnSize.y }, btnSize), "Play");

	m_exitButton = rgc::Button(rgc::Bounds({ game.getTexSize().x / 2 - btnSize.x / 2,
		game.getTexSize().y / 2 }, btnSize), "Exit");
}

void GUI::initGame(Game& game)
{
	Vector2 btnSize = { 64.f, 16.f };
	m_menuButton = rgc::Button(rgc::Bounds({ game.getTexSize().x - btnSize.x, 0 }, btnSize), "Menu");
}

void GUI::drawEnergyBar(int cellSize, int rows, float energy)
{
	const Rectangle rec = { 0, 0, cellSize * rows, cellSize / 2 };

	rlPushMatrix();

	rlTranslatef(cellSize / 2 - rec.height / 2, (1 + rows) * cellSize, 0);
	rlRotatef(-90, 0, 0, 1);

	m_energyBar = rgc::ProgressBar(rgc::Bounds(0, 0, cellSize * rows, cellSize / 2), NULL, NULL, energy, 0.f, 100.f);
	m_energyBar.Show();

	rlPopMatrix();
}

void GUI::drawGame(Game& game, int cellSize, int rows, float energy)
{
	drawEnergyBar(cellSize, rows, energy);
	if (m_menuButton.Show())
	{
		game.setState(std::make_unique<MenuState>(game));
	}
}

void GUI::drawMenu(Game& game)
{
	if (m_exitButton.Show())
	{
		exit(0);
	}

	if (m_playButton.Show())
	{
		game.setState(std::make_unique<PlayState>(game));
	}
}