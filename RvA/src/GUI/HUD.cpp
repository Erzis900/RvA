#include "GUI/HUD.h"
#include "GUI/GUI.h"
#include <rlgl.h>
#include <raygui.h>
#include "constants.h"

HUD::HUD(Atlas& atlas, GUI& gui) : m_atlas(atlas), m_gui(gui)
{
}

void HUD::draw()
{
	drawScrapAmount();
	drawBatteryCharge();
	drawDefenders();
	drawMenuButtons();
	
	if (m_data.drawPause)
	{
		drawPause();
	}
}

void HUD::drawHPBar()
{

}

void HUD::onPauseButtonPressed(std::function<void()> callback)
{
	m_onPauseButtonPressedCallback = std::move(callback);
}

void HUD::onResumeButtonPressed(std::function<void()> callback)
{
	m_onResumeButtonPressedCallback = std::move(callback);
}

void HUD::onMenuButtonPressed(std::function<void()> callback)
{
	m_onMenuButtonPressedCallback = std::move(callback);
}

void HUD::drawScrapAmount()
{
	DrawText(std::to_string(m_data.scrapsAmount).c_str(), 10, 10, 5, ORANGE);
}

void HUD::drawBatteryCharge()
{
	const Rectangle rec = { 0.f, 0.f, float(CELL_SIZE * ROWS), float(CELL_SIZE / 2) };

	rlPushMatrix();

	rlTranslatef(float(CELL_SIZE / 2 - rec.height / 2), float((1 + ROWS) * CELL_SIZE), 0.f);
	rlRotatef(-90, 0, 0, 1);

	GuiProgressBar(rec, nullptr, nullptr, &m_data.batteryCharge, 0.f, 100.f);

	rlPopMatrix();
}

void HUD::drawDefenders()
{
	m_defenderHover = false;

	auto i = 0;
	for (auto& defender : m_data.defenders)
	{
		Vector2 position = { float(CELL_SIZE + CELL_SIZE * i), 0.f };
		m_atlas.drawSprite(defender.spriteInfo, position);

		Rectangle rect = { position.x, position.y, float(CELL_SIZE), float(CELL_SIZE) };

		if (m_data.selectedDefender == defender.type)
		{
			DrawRectangleLinesEx(rect, 1, GREEN);
		}

		if (CheckCollisionPointRec(GetMousePosition(), rect) && !m_data.drawPause)
		{
			m_defenderHover = true;
			DrawRectangleLinesEx(rect, 1, SKYBLUE);
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
			{
				m_data.selectedDefender = defender.type;
			}
		}

		std::string costText = std::to_string(defender.cost);
		DrawText(costText.c_str(), int(position.x), int(position.y), 10, WHITE);

		++i;
	}

	if (m_defenderHover)
	{
		m_gui.setCursor(CursorType::Hover);
	}
	else
	{
		m_gui.setCursor(CursorType::Point);
	}
}

void HUD::drawMenuButtons()
{
	Vector2 btnSize = { 64.f, 16.f };
	if (GuiButton({ TEX_WIDTH - btnSize.x, 0, btnSize.x, btnSize.y }, "Menu") && !m_data.drawPause)
	{
		m_onMenuButtonPressedCallback();
	}

	if (GuiButton({ TEX_WIDTH - btnSize.x, btnSize.y, btnSize.x, btnSize.y }, "Pause") && !m_data.drawPause)
	{
		m_onPauseButtonPressedCallback();
	}
}

void HUD::drawPause()
{
	DrawRectangle(0, 0, TEX_WIDTH, TEX_HEIGHT, {0, 0, 0, 100});
	m_gui.drawText({ "Paused", 20, WHITE, { {}, GUIAlignmentH::Center, GUIAlignmentV::Center} });

	auto btnSize = Vector2{ 100.f, 30.f };
	if (m_gui.drawButton({ "Resume", btnSize, { {0, 60}, GUIAlignmentH::Center, GUIAlignmentV::Center } }))
	{
		m_onResumeButtonPressedCallback();
	}
}
