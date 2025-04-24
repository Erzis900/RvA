#include "GUI.h"
#include "Game.h"
#include "states/MenuState.h"
#include "states/PlayState.h"
#include "rlgl.h"
#include <raygui.h>

GUI::GUI(Game& game)
	:m_game(game), m_selectedDefender(DefenderType::None)
{
}

const char* getSpriteName(DefenderType type)
{
	switch (type)
	{
	case DefenderType::Solar: return "solar_idle";
	case DefenderType::Shooter: return "shooter_idle";
	case DefenderType::Catapult: return "catapult_idle";
	case DefenderType::Lasertron: return "lasertron_idle";
	default: return "";
	}
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

void GUI::drawDefenders(int cellSize)
{
	DefenderType types[] = {
		DefenderType::Solar,
		DefenderType::Shooter,
		DefenderType::Catapult,
		DefenderType::Lasertron
	};

	for (int i = 0; i < int(DefenderType::None); i++)
	{
		Vector2 position = { float(cellSize + cellSize * i), 0.f };
		m_game.getAtlas().drawSprite(getSpriteName(types[i]), position);

		Rectangle rect = { position.x, position.y, float(cellSize), float(cellSize) };

		if (m_selectedDefender == types[i])
		{
			DrawRectangleLinesEx(rect, 1, GREEN);
		}

		if (CheckCollisionPointRec(GetMousePosition(), rect))
		{
			DrawRectangleLinesEx(rect, 1, SKYBLUE);
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				m_selectedDefender = types[i];
			}
		}
	}
}

void GUI::drawCosts(int cellSize, DefenderManager& defenderManager)
{
	for (int i = 0; i < int(DefenderType::None); i++)
	{
		Vector2 position = { float(cellSize + cellSize * i), 0.f };
		std::string costText = std::to_string(defenderManager.getCosts()[i]);
		DrawText(costText.c_str(), int(position.x), int(position.y), 10, WHITE);
	}
}

void GUI::drawGame(int cellSize, int rows, float energy, DefenderManager& defenderManager)
{
	drawEnergyBar(cellSize, rows, energy);
	drawDefenders(cellSize);
	drawCosts(cellSize, defenderManager);
	DrawText(TextFormat("%d", batteries), 10, 10, 5, ORANGE);

	Vector2 btnSize = { 64.f, 16.f };
	if (GuiButton({ m_game.getTexSize().x - btnSize.x, 0, btnSize.x, btnSize.y}, "Menu"))
	{
		m_game.setState(std::make_unique<MenuState>(m_game));
	}
}

void GUI::drawCursor()
{
	m_game.getAtlas().drawSprite("mouse_cursor_point", { GetMousePosition().x - 10, GetMousePosition().y - 5});
}

void GUI::drawHp(int cellSize, int hp, int maxHp, Vector2 pos)
{
	float barWidth = float(cellSize);
	float barHeight = 3.f;
	float hpPercent = float(hp) / float(maxHp);

	Vector2 barPos = { pos.x, pos.y + cellSize };
	Rectangle bg = { barPos.x, barPos.y, barWidth, barHeight };
	Rectangle fg = { barPos.x, barPos.y, barWidth * hpPercent, barHeight };

	DrawRectangleRec(bg, DARKGRAY);
	DrawRectangleRec(fg, GREEN);
}