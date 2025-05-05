#include "GUI/HUD.h"
#include "GUI/GUI.h"

#include <rlgl.h>
#include <raygui.h>

#include "constants.h"

const float defenderSize = CELL_SIZE;
const float defenderPadding = 5.f;

HUD::HUD(GUI& gui) : m_gui(gui)
{
	m_data.progressBars.reserve(128);

	m_screen = m_gui.buildScreen("HUD")
		.stack({ .orientation = GUIOrientation::Vertical, .padding = 5, .size = { 100.f, autoSize } })
            .stack({ .orientation = GUIOrientation::Horizontal, .padding = 5, .hAlign = HAlign::Stretch, .size = { autoSize, 20.f }, .pos = { 5, 0 } })
				.small_text({ .text = "SCRAPS:", .color = WHITE, .hAlign = HAlign::Left, .vAlign = VAlign::Center })
				.small_text({ .text = "0", .color = ORANGE, .hAlign = HAlign::Left, .vAlign = VAlign::Center }, &m_scrapTextHandle)
			.end()
            .stack({ .orientation = GUIOrientation::Horizontal, .padding = 5, .hAlign = HAlign::Stretch, .size = { autoSize, 20.f }, .pos = { 5, 0 } })
				.small_text({ .text = "BATTERY:", .color = WHITE, .hAlign = HAlign::Left, .vAlign = VAlign::Center })
				.small_text({ .text = "100%", .color = ORANGE, .hAlign = HAlign::Left, .vAlign = VAlign::Center }, &m_batteryTextHandle)
			.end()
		.end()
		.custom({
            .pos = { 5, 13 },
			.draw = std::bind_front(&HUD::drawBatteryCharge, this),
			.measure = [](const Vector2& size) { return Vector2{ float(CELL_SIZE / 2), float(CELL_SIZE * ROWS) }; },
			.vAlign = VAlign::Center,
		})
		.border({ .color = BLACK, .thickness = 2, .pos = { 0, defenderPadding }, .padding = { 5, 5 }, .hAlign = HAlign::Center})
			.custom({
				.draw = std::bind_front(&HUD::drawDefenders, this),
				.measure = std::bind_front(&HUD::measureDefenders, this),
				.hAlign = HAlign::Center,
                .vAlign = VAlign::Center
				})
		.end()
		.custom({
			.draw = std::bind_front(&HUD::drawProgressBars, this),
			.vAlign = VAlign::Center
		})
	.screen();
    m_screen->setVisible(false);
}

void HUD::setEnable(bool enabled)
{
    m_isEnabled = enabled;
}

Color calculateBatteryColor(float batteryCharge) {
	float t = 1.0f - batteryCharge / 100.0f;

	if (t < 0.5f) {
		// Green to Orange
		return ColorLerp(GREEN, ORANGE, t * 2.0f);
	} else {
		// Orange to Red
		return ColorLerp(ORANGE, RED, (t - 0.5f) * 2.0f);
	}
}

void HUD::update(float dt)
{
	auto scrapsText = std::to_string(m_data.scrapsAmount);
	m_screen->getText(m_scrapTextHandle).text = scrapsText.c_str();
	auto& batteryText = m_screen->getText(m_batteryTextHandle);
	batteryText.text = TextFormat("%d%%", int(m_data.batteryCharge));
    batteryText.color = calculateBatteryColor(m_data.batteryCharge);
}

void HUD::setVisible(bool visible)
{
    m_screen->setVisible(visible);
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

void HUD::drawBatteryCharge(Atlas& atlas, const Rectangle& bounds)
{
	Rectangle rect = bounds;

    rect.height = m_data.batteryCharge / 100.f * bounds.height;
	rect.y += bounds.height - rect.height;

    DrawRectangleRec(rect, calculateBatteryColor(m_data.batteryCharge));
	auto padding = 2;
	DrawRectangleLinesEx({ bounds.x - padding, bounds.y - padding, bounds.width + padding * 2, bounds.height + padding * 2 }, 1, SKYBLUE);

	rlPopMatrix();
}

Vector2 HUD::measureDefenders(const Vector2& availableSize)
{
	auto size = Vector2{0, defenderSize};
    if (m_data.defenders.empty()) {
        return size;
    }

	for (auto& defender : m_data.defenders)
	{
        size.x += defenderSize + defenderPadding;
	}
	size.x -= defenderPadding;

	return size;
}

void HUD::drawDefenders(Atlas& atlas, const Rectangle& bounds)
{
	m_defenderHover = false;

	auto i = 0;
	for (auto& defender : m_data.defenders)
	{
		Vector2 position = { bounds.x + defenderSize * i + defenderPadding * i, bounds.y };

		float halfPadding = defenderPadding / 2;
		Rectangle rect = { position.x - halfPadding, position.y - halfPadding, defenderSize + defenderPadding, defenderSize + defenderPadding };

		if(m_isEnabled)
		{
			if (m_data.selectedDefender == defender.type) {
				DrawRectangleLinesEx(rect, 1, GREEN);
			}

			if (CheckCollisionPointRec(GetMousePosition(), rect)) {
				m_defenderHover = true;
				DrawRectangleRec(rect, Fade(SKYBLUE, 0.5f));
				if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					m_data.selectedDefender = defender.type;
					m_onDefenderSelectedCallbacks.executeCallbacks();
				}
			}
		}

		atlas.drawSprite(defender.spriteInfo, position);
		std::string costText = defender.cost > 0 ? std::to_string(defender.cost) : "--";
		auto size = MeasureText("00", 10);
		DrawRectangleRec({ position.x - 1, position.y - 1, (float)size + 2, 9 }, Fade(BLACK, 0.5f));
		DrawText(costText.c_str(), int(position.x), int(position.y), 10, defender.cost <= m_data.scrapsAmount ? WHITE : RED);

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

void HUD::drawProgressBars(Atlas& atlas, const Rectangle& bounds)
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
