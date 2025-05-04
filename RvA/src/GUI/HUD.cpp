#include "GUI/HUD.h"
#include "GUI/GUI.h"

#include <rlgl.h>
#include <raygui.h>

#include "constants.h"

HUD::HUD(GUI& gui) : m_gui(gui)
{
	m_data.progressBars.reserve(128);
}

void HUD::setEnable(bool enabled)
{
    m_isEnabled = enabled;
}

void HUD::draw(Atlas& atlas)
{
	drawScrapAmount();
	drawBatteryCharge();
	drawDefenders(atlas);
	drawProgressBars();
}

void HUD::drawProgressBar(float value, float max, const Vector2& pos, Color bkgColor, Color fillColor)
{
	float barWidth = float(CELL_SIZE);
	float barHeight = 3.f;
	float valPercent = value / max;

	Vector2 barPos = { pos.x, pos.y + CELL_SIZE };
	Rectangle bg = { barPos.x, barPos.y, barWidth, barHeight };
	Rectangle fg = { barPos.x, barPos.y, barWidth * valPercent, barHeight };

	DrawRectangleRec(bg, bkgColor);
	DrawRectangleRec(fg, fillColor);
}

CallbackHandle HUD::onDefenderSelected(std::function<void()> callback)
{
    return m_onDefenderSelectedCallbacks.registerCallback(std::move(callback));
}

void HUD::clear()
{
    m_data.defenders.clear();
    m_data.progressBars.clear();
    m_data.scrapsAmount = 0;
    m_data.batteryCharge = 0.f;
    m_data.selectedDefender.reset();
}

void HUD::drawScrapAmount()
{
	auto scrapsText = std::to_string(m_data.scrapsAmount);
	m_gui.drawText({
		scrapsText.c_str(),
		5,
		ORANGE,
		{{10, 10}, GUIAlignmentH::Left, GUIAlignmentV::Top} });
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

void HUD::drawDefenders(Atlas& atlas)
{
	m_defenderHover = false;

	auto i = 0;
	for (auto& defender : m_data.defenders)
	{
		Vector2 position = { float(CELL_SIZE + CELL_SIZE * i), 0.f };
		atlas.drawSprite(defender.spriteInfo, position);

		Rectangle rect = { position.x, position.y, float(CELL_SIZE), float(CELL_SIZE) };

		if(m_isEnabled)
		{
			if (m_data.selectedDefender == defender.type) {
				DrawRectangleLinesEx(rect, 1, GREEN);
			}

			if (CheckCollisionPointRec(GetMousePosition(), rect)) {
				m_defenderHover = true;
				DrawRectangleLinesEx(rect, 1, SKYBLUE);
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					m_data.selectedDefender = defender.type;
					m_onDefenderSelectedCallbacks.executeCallbacks();
				}
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

void HUD::drawProgressBars()
{
	for (auto& progressBar : m_data.progressBars)
	{
		drawProgressBar(
			progressBar.value,
			progressBar.max,
			progressBar.position,
			progressBar.bkgColor,
			progressBar.fillColor
		);
	}
}
