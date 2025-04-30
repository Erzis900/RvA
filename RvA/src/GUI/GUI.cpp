#include "GUI.h"
#include "Game.h"
#include "states/MenuState.h"
#include "states/PlayState.h"
#include "rlgl.h"
#include <raygui.h>
#include "constants.h"

GUI::GUI(Game& game)
	:m_game(game), m_selectedDefender(DefenderType::None), m_defenderHover(false)
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

void GUI::reset()
{
	m_selectedDefender = DefenderType::None;
}

void GUI::drawEnergyBar(float energy)
{
	const Rectangle rec = { 0.f, 0.f, float(CELL_SIZE * ROWS), float(CELL_SIZE / 2) };

	rlPushMatrix();

	rlTranslatef(float(CELL_SIZE / 2 - rec.height / 2), float((1 + ROWS) * CELL_SIZE), 0.f);
	rlRotatef(-90, 0, 0, 1);

	GuiProgressBar(rec, nullptr, nullptr, &energy, 0.f, 100.f);

	rlPopMatrix();
}

void GUI::drawDefenders()
{
	m_defenderHover = false;

	auto i = 0;
	for (auto& [type, info] : m_game.getDefenderRegistry().getDefenderInfos())
	{
		Vector2 position = { float(CELL_SIZE + CELL_SIZE * i), 0.f };
		m_game.getAtlas().drawSprite(info.spriteEnabled.c_str(), position);

		Rectangle rect = { position.x, position.y, float(CELL_SIZE), float(CELL_SIZE) };

		if (m_selectedDefender == type)
		{
			DrawRectangleLinesEx(rect, 1, GREEN);
		}

		if (CheckCollisionPointRec(GetMousePosition(), rect))
		{
			m_defenderHover = true;
			DrawRectangleLinesEx(rect, 1, SKYBLUE);
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				m_selectedDefender = type;
			}
		}
		++i;
	}
}

void GUI::drawCosts()
{
	auto i = 0;
	for (auto& [type, info] : m_game.getDefenderRegistry().getDefenderInfos())
	{
		Vector2 position = { float(CELL_SIZE + CELL_SIZE * i), 0.f };
		std::string costText = std::to_string(info.cost);
		DrawText(costText.c_str(), int(position.x), int(position.y), 10, WHITE);
		++i;
	}
}

void GUI::drawGame(float energy, float batteries)
{
	drawEnergyBar(energy);
	drawDefenders();
	drawCosts();
	DrawText(TextFormat("%d", static_cast<int>(batteries)), 10, 10, 5, ORANGE);

	Vector2 btnSize = { 64.f, 16.f };
	if (GuiButton({ TEX_WIDTH - btnSize.x, 0, btnSize.x, btnSize.y}, "Menu"))
	{
		m_game.setState(std::make_unique<MenuState>());
	}

	if (GuiButton({ TEX_WIDTH - btnSize.x, btnSize.y, btnSize.x, btnSize.y }, "Pause"))
	{
		m_paused = !m_paused;
	}
	if (m_paused) DrawText("Paused", int(TEX_WIDTH / 2 - MeasureText("Paused", 20) / 2), int(TEX_HEIGHT / 2 - 10), 20, WHITE);
}

void GUI::drawCursor()
{
	if (!m_defenderHover)
	{
		m_game.getAtlas().drawSprite("mouse_cursor_point", { GetMousePosition().x - 10, GetMousePosition().y - 5});
	}
	else
	{
		m_game.getAtlas().drawSprite("mouse_cursor_hover", { GetMousePosition().x - 10, GetMousePosition().y - 5 });
	}
}

void GUI::drawHp(int hp, int maxHp, Vector2 pos)
{
	float barWidth = float(CELL_SIZE);
	float barHeight = 3.f;
	float hpPercent = float(hp) / float(maxHp);

	Vector2 barPos = { pos.x, pos.y + CELL_SIZE };
	Rectangle bg = { barPos.x, barPos.y, barWidth, barHeight };
	Rectangle fg = { barPos.x, barPos.y, barWidth * hpPercent, barHeight };

	DrawRectangleRec(bg, DARKGRAY);
	DrawRectangleRec(fg, GREEN);
}