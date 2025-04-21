#include "GUI.h"
#include "Game.h"
#include "states/MenuState.h"
#include "states/PlayState.h"
#include "rlgl.h"
#include <raygui.h>

GUI::GUI(Game& game)
	:m_game(game)
{
}

void GUI::drawEnergyBar(int cellSize, int rows, float energy)
{
	const Rectangle rec = { 0.f, 0.f, float(cellSize * rows), float(cellSize / 2) };

	rlPushMatrix();

	rlTranslatef(float(cellSize / 2 - rec.height / 2), float((1 + rows) * cellSize), 0.f);
	rlRotatef(-90, 0, 0, 1);

	GuiProgressBar(rec, nullptr, nullptr, &energy, 0.f, 100.f);

	rlPopMatrix();
}

void GUI::drawDefenders()
{
	Atlas& atlas = m_game.getAtlas();
	//atlas.drawSprite("")
}

void GUI::drawGame(int cellSize, int rows, float energy)
{
	drawEnergyBar(cellSize, rows, energy);

	Vector2 btnSize = { 64.f, 16.f };
	if (GuiButton({ m_game.getTexSize().x - btnSize.x, 0, btnSize.x, btnSize.y}, "Menu"))
	{
		m_game.setState(std::make_unique<MenuState>(m_game));
	}
}

void GUI::drawMenu()
{
	Vector2 btnSize = { 200.f, 60.f };
	if (GuiButton({ m_game.getTexSize().x / 2 - btnSize.x / 2, m_game.getTexSize().y / 2, btnSize.x, btnSize.y }, "Exit"))
	{
		exit(0);
	}

	if (GuiButton({ m_game.getTexSize().x / 2 - btnSize.x / 2, m_game.getTexSize().y / 2 - btnSize.y, btnSize.x, btnSize.y }, "Play"))
	{
		m_game.setState(std::make_unique<PlayState>(m_game));
	}
}

void GUI::drawCursor()
{
	m_game.getAtlas().drawSprite("mouse_cursor_point", { GetMousePosition().x - 10, GetMousePosition().y - 5});
}