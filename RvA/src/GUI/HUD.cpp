#include "GUI/HUD.h"

#include "GUI/GUI.h"
#include "GUI/LayoutHelper.h"
#include "constants.h"

#include <algorithm>
#include <raygui.h>
#include <raymath.h>
#include <rlgl.h>

const float defenderSize = CELL_SIZE;
const float defenderHSize = CELL_SIZE * 0.5f;
const float defenderSlotSize = 52;
const float defenderSlotHSize = defenderSlotSize * 0.5f;
const float defenderPadding = 5.f;

HUD::HUD(GUI& gui) : m_gui(gui) {
	m_data.progressBars.reserve(128);

	auto* bottomBarSpriteInfo = m_gui.getAtlas().getSpriteInfo("ui_bottom_bar");

	// clang-format off
	m_screen = m_gui.buildScreen("HUD")
		// TODO(Gerark) - Due to Texture Bleeding we're forced to set this strange padding and offset. Remove them as soon as the issue is solved.
		.stack({ .orientation = GUIOrientation::Horizontal, .padding = { -2, 0 }, .hAlign = HAlign::Stretch, .vAlign = VAlign::Bottom, .size = { autoSize, 69.f }, .pos = { -1, -1 } })
			.image({ .sprite = bottomBarSpriteInfo })
			.image({ .sprite = bottomBarSpriteInfo, .flip = Flip::Horizontal })
		.end()
		.small_text({ .text = "", .color = WHITE, .hAlign = HAlign::Right, .vAlign = VAlign::Bottom, .pos = { 5, 5 } }, &m_levelNameHandle)
		.stack({ .orientation = GUIOrientation::Vertical, .padding = 5, .vAlign = VAlign::Bottom, .size = { 100.f, autoSize }, .pos = { 10, 4 } })
            .stack({ .orientation = GUIOrientation::Horizontal, .padding = 5, .hAlign = HAlign::Stretch, .size = { autoSize, 20.f } })
				.small_text({ .text = "SCRAPS:", .color = WHITE, .hAlign = HAlign::Left, .vAlign = VAlign::Center })
				.small_text({ .text = "0", .color = ORANGE, .hAlign = HAlign::Left, .vAlign = VAlign::Center }, &m_scrapTextHandle)
			.end()
            .stack({ .orientation = GUIOrientation::Horizontal, .padding = 5, .hAlign = HAlign::Stretch, .size = { autoSize, 20.f } })
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
		.custom({
			.pos = { 0, 10 },
			.draw = std::bind_front(&HUD::drawDefenders, this),
			.measure = std::bind_front(&HUD::measureDefenders, this),
			.hAlign = HAlign::Center,
            .vAlign = VAlign::Bottom
			})
		.custom({
			.draw = std::bind_front(&HUD::drawProgressBars, this),
			.vAlign = VAlign::Center
		})
	.screen();
	// clang-format on
	m_screen->setVisible(false);
}

void HUD::setEnable(bool enabled) {
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

void HUD::update(float dt) {
	auto scrapsText = std::to_string(m_data.scrapsAmount);
	m_screen->getText(m_scrapTextHandle).text = scrapsText.c_str();
	auto& batteryText = m_screen->getText(m_batteryTextHandle);
	batteryText.text = TextFormat("%d%%", int(m_data.batteryCharge / m_data.maxBatteryCharge * 100));
	batteryText.color = calculateBatteryColor(m_data.batteryCharge);
	auto& levelName = m_screen->getText(m_levelNameHandle);
	levelName.text = m_data.levelName;

	for (auto& defender : m_data.defenders) {
		if (defender.isHover) {
			defender.animation.update(dt);
		}
	}
}

void HUD::setVisible(bool visible) {
	m_screen->setVisible(visible);
}

void HUD::drawProgressBar(float value, float max, const Vector2& pos, Color bkgColor, Color fillColor) {
	float barWidth = float(CELL_SIZE);
	float barHeight = 3.f;
	float valPercent = value / max;

	Vector2 barPos = {pos.x, pos.y + CELL_SIZE};
	Rectangle bg = {barPos.x, barPos.y, barWidth, barHeight};
	Rectangle fg = {barPos.x, barPos.y, barWidth * valPercent, barHeight};

	DrawRectangleRec(bg, bkgColor);
	DrawRectangleRec(fg, fillColor);
}

CallbackHandle HUD::onDefenderSelected(std::function<void(int)> callback) {
	return m_onDefenderSelectedCallbacks.registerCallback(std::move(callback));
}

void HUD::clear() {
	m_data.defenders.clear();
	m_data.progressBars.clear();
	m_data.scrapsAmount = 0;
	m_data.batteryCharge = 0.f;
	m_data.maxBatteryCharge = 0.f;
	m_data.selectedDefenderIndex.reset();
	m_data.numberOfEnemiesDefeated = 0;
	m_data.occupiedCells.clear();
	m_data.levelName = "";
}

void HUD::drawBatteryCharge(Atlas& atlas, const Rectangle& bounds) {
	Rectangle rect = bounds;

	rect.height = m_data.batteryCharge / m_data.maxBatteryCharge * bounds.height;
	rect.y += bounds.height - rect.height;

	DrawRectangleRec(rect, calculateBatteryColor(m_data.batteryCharge));
	auto padding = 2;
	DrawRectangleLinesEx({bounds.x - padding, bounds.y - padding, bounds.width + padding * 2, bounds.height + padding * 2}, 1, SKYBLUE);

	rlPopMatrix();
}

Vector2 HUD::measureDefenders(const Vector2& availableSize) {
	auto size = Vector2{0, defenderSlotSize};
	if (m_data.defenders.empty()) {
		return size;
	}

	for (auto& defender : m_data.defenders) {
		size.x += defenderSlotSize + defenderPadding;
	}
	size.x -= defenderPadding;

	return size;
}

void HUD::drawDefenders(Atlas& atlas, const Rectangle& bounds) {
	bool isAnyDefenderHovered = false;

	auto i = 0;
	for (auto& defender : m_data.defenders) {
		Vector2 position = {bounds.x + defenderSlotSize * i + defenderPadding * i, bounds.y};

		float halfPadding = defenderPadding / 2;
		Rectangle frameRect = {position.x - halfPadding, position.y - halfPadding, defenderSlotSize + defenderPadding, defenderSlotSize + defenderPadding};

		const bool canBuild = defender.canAfford && defender.cooldown <= 0;

		defender.isHover = false;
		bool isSelected = false;
		if (m_isEnabled) {
			isSelected = (m_data.selectedDefenderIndex == i);

			if (CheckCollisionPointRec(GetMousePosition(), frameRect)) {
				isAnyDefenderHovered = true;
				defender.isHover = true;
				if (canBuild && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
					m_data.selectedDefenderIndex = i;
					m_onDefenderSelectedCallbacks.executeCallbacks(*m_data.selectedDefenderIndex);
				}
			}
		}

		const SpriteInfo* spriteInfo = nullptr;
		if (isSelected) {
			spriteInfo = atlas.getSpriteInfo("ui_defender_slot_sel");
		} else if (!canBuild) {
			spriteInfo = atlas.getSpriteInfo("ui_defender_slot_no");
		} else if (defender.isHover) {
			spriteInfo = atlas.getSpriteInfo("ui_defender_slot_yes");
		} else {
			spriteInfo = atlas.getSpriteInfo("ui_defender_slot");
		}
		atlas.drawSprite(spriteInfo, position);

		auto rect = LayoutHelper::arrangePositionAndSize({0, -2}, {14, 6}, frameRect, HAlign::Center, VAlign::Center);
		DrawEllipse(rect.x + 7, rect.y + 3, 14, 6, Fade(BLACK, 0.1f));

		rect = LayoutHelper::arrangePositionAndSize({0, -5}, {defenderSize, defenderSize}, frameRect, HAlign::Center, VAlign::Top);
		atlas.drawSprite(defender.animation.getSpriteInfo(), {rect.x, rect.y}, canBuild ? defender.animation.getCurrentFrame() : 0, Flip::None, canBuild ? WHITE : Fade(GRAY, 0.5));

		if (defender.cooldown > 0) {
			auto progress = defender.cooldown / defender.maxCooldown;

			DrawCircleSector({position.x + defenderSlotHSize, position.y + defenderSlotHSize}, 20, -90, -90 - 360 * progress, 100, Fade(BLACK, 0.5f));

			auto cooldownText = TextFormat("%.1fs", defender.cooldown);
			rect = LayoutHelper::arrangePositionAndSize(cooldownText, 10, {0, 0}, 1, frameRect, HAlign::Center, VAlign::Center);
			DrawRectangleRec({rect.x - 2, rect.y - 2, rect.width + 4, rect.height + 2}, Fade(BLACK, 0.5f));
			DrawText(cooldownText, rect.x, rect.y, rect.height, WHITE);
		}

		std::string costText = defender.cost > 0 ? std::to_string(defender.cost) : "--";
		auto cooldownText = TextFormat("%.1f", defender.cooldown);
		rect = LayoutHelper::arrangePositionAndSize(costText.c_str(), 10, {0, 0}, 1, frameRect, HAlign::Center, VAlign::Bottom);
		DrawRectangleRec({rect.x - 2, rect.y - 2, rect.width + 4, rect.height + 2}, Fade(BLACK, 0.5f));
		DrawText(costText.c_str(), rect.x, rect.y, rect.height, defender.canAfford ? WHITE : RED);

		++i;
	}

	if (isAnyDefenderHovered) {
		m_gui.setCursor(CursorType::Hover);
	} else {
		m_gui.setCursor(CursorType::Point);
	}

	if (m_data.selectedDefenderIndex) {
		auto mousePos = GetMousePosition();
		auto [row, column] = getCoordinates(mousePos);
		auto position = getSnappedPosition(mousePos);

		if (row >= 0 && row < ROWS && column >= 0 && column < COLS) {
			const bool isOccupied = std::ranges::any_of(m_data.occupiedCells, [row, column](const auto& cell) { return cell.row == row && cell.column == column; });
			DrawRectangleRec({position.x, position.y, CELL_SIZE, CELL_SIZE}, isOccupied ? Fade(RED, 0.5f) : Fade(SKYBLUE, 0.5f));
		}

		position = Vector2Add(mousePos, {-defenderSize * 0.7f, -defenderSize * 0.7f});
		auto& defender = m_data.defenders[*m_data.selectedDefenderIndex];
		atlas.drawSprite(defender.animation.getSpriteInfo(), position, 0, None, Fade(WHITE, 0.7f));
	}
}

void HUD::drawProgressBars(Atlas& atlas, const Rectangle& bounds) {
	for (auto& progressBar : m_data.progressBars) {
		if (progressBar.value != progressBar.max) {
			drawProgressBar(progressBar.value, progressBar.max, progressBar.position, progressBar.bkgColor, progressBar.fillColor);
		}
	}
}
