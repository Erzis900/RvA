#include "GUI/HUD.h"

#include "GUI/GUI.h"
#include "GUI/LayoutHelper.h"
#include "ResourceSystem.h"
#include "constants.h"

#include <algorithm>
#include <raymath.h>

const float defenderSize = CELL_SIZE;
const float defenderHSize = CELL_SIZE * 0.5f;
const float defenderSlotSize = 52;
const float defenderSlotHSize = defenderSlotSize * 0.5f;
const float defenderPadding = 5.f;
const float maxBatteryFill = (defenderSize * ROWS) - CELL_SIZE * 0.5f;
const float totalTimelineFill = 200.f;

HUD::HUD(GUI& gui, ResourceSystem& resourceSystem) : m_gui(gui), m_resourceSystem(resourceSystem) {
	m_data.progressBars.reserve(128);

	auto* bottomBarSpriteInfo = m_gui.getAtlas().getSpriteInfo("ui_bottom_bar");
	auto* batteryTop = m_gui.getAtlas().getSpriteInfo("ui_battery_top");
	auto* batteryBottom = m_gui.getAtlas().getSpriteInfo("ui_battery_bottom");
	auto* batteryMiddle = m_gui.getAtlas().getSpriteInfo("ui_battery_middle");
	auto* timelineSide = m_gui.getAtlas().getSpriteInfo("ui_timeline_side");
	auto timelineSideSize = Vector2{totalTimelineFill + timelineSide->frames[0].width * 2, static_cast<float>(timelineSide->frames[0].height)};

	// clang-format off
	m_screen = m_gui.buildScreen("HUD")
		// Bottom Bar
		// TODO(Gerark) - Due to Texture Bleeding we're forced to set this strange padding and offset. Remove them as soon as the issue is solved.
		.stack({ .orientation = GUIOrientation::Horizontal, .padding = { -2, 0 }, .hAlign = HAlign::Stretch, .vAlign = VAlign::Bottom, .size = Vec2{ autoSize, 69.f }, .pos = { -1, -1 } })
			.image({ .sprite = bottomBarSpriteInfo })
			.image({ .sprite = bottomBarSpriteInfo, .flip = Flip::Horizontal })
		.end()
		// Level Name
		.small_text({ .text = "", .color = WHITE, .hAlign = HAlign::Right, .vAlign = VAlign::Bottom, .pos = { 5, 5 } }, &m_levelNameHandle)
		// Battery & Scraps
		.stack({ .orientation = GUIOrientation::Horizontal, .padding = 10, .vAlign = VAlign::Bottom, .pos = { 10, 0 }, .alignContent = ContentAlign::Center }, &m_batteryAndScrapsHandle)
            .stack({ .orientation = GUIOrientation::Vertical, .padding = 0, .alignContent = ContentAlign::Center, .sideAlignContent = ContentAlign::Center })
				.image({ .sprite = m_gui.getAtlas().getSpriteInfo("scrap_idle"), .hAlign = HAlign::Center, .fit = Fit::Ignore })
				.small_text({ .text = "0", .color = ORANGE}, &m_scrapTextHandle)
				.small_text({ .text = "SCRAPS", .color = WHITE})
			.end()
            .stack({ .orientation = GUIOrientation::Vertical, .padding = 0, .alignContent = ContentAlign::Center, .sideAlignContent = ContentAlign::Center })
				.image({ .sprite = m_gui.getAtlas().getSpriteInfo("battery_mini"), .hAlign = HAlign::Center, .fit = Fit::Ignore }, &m_batteryStatIconHandle)
				.stack({ .orientation = GUIOrientation::Horizontal, .padding = { 5, 0 }, .alignContent = ContentAlign::Center, .sideAlignContent = ContentAlign::Center })
					.small_text({ .text = "100%", .color = ORANGE }, &m_batteryTextHandle)
					.custom({
						.draw = std::bind_front(&HUD::drawBatteryTrend, this),
						.measure = std::bind_front(&HUD::measureBatteryTrend, this)
					})
				.end()
				.small_text({ .text = "BATTERY", .color = WHITE })
			.end()
		.end()
		// Big Battery
		.stack({
			.orientation = GUIOrientation::Vertical, .hAlign = HAlign::Left, .vAlign = VAlign::Bottom,
			.size = Vec2{ 64, autoSize }, .pos = { 0, 10 }, .alignContent = ContentAlign::Center, .sideAlignContent = ContentAlign::Center })
			.image({ .sprite = batteryTop }, &m_batteryTipHandle)
			.image({ .size = Vector2{32, maxBatteryFill}, .sprite = batteryMiddle, .textureFillMode = TextureFillMode::Repeat }, &m_batteryFillHandle)
			.image({ .sprite = batteryBottom }, &m_batteryBottomHandle)
		.end()
		.stack({
			.orientation = GUIOrientation::Vertical, .hAlign = HAlign::Left, .vAlign = VAlign::Bottom,
			.size = Vec2{ 64, autoSize }, .pos = { 5, 81 }, .alignContent = ContentAlign::End, .sideAlignContent = ContentAlign::Center })
			.shape({ .size = { 4, maxBatteryFill }, .color = Fade(BLACK, 0.25f), .type = ShapeType::Rectangle, .roundness = 5 })
		.end()
		.stack({
			.orientation = GUIOrientation::Vertical, .hAlign = HAlign::Left, .vAlign = VAlign::Bottom,
			.size = Vec2{ 64, autoSize }, .pos = { 5, 81 }, .alignContent = ContentAlign::End, .sideAlignContent = ContentAlign::Center })
			.shape({ .size = { 4, maxBatteryFill }, .color = Fade(GREEN, 0.75f), .type = ShapeType::Rectangle, .roundness = 5 }, &m_batteryIndicatorHandle)
		.end()
		// Plate
		.border({ .color = Fade(BLACK, 0.5), .bkgColor = Fade(BLACK, 0.5), .pos = {0, 70}, .padding = {20, 5}, .hAlign = HAlign::Center, .vAlign = VAlign::Bottom }, &m_plateContainerHandle)
			.stack({ .orientation = GUIOrientation::Vertical, .padding = {0, 5}, .hAlign = HAlign::Center, .vAlign = VAlign::Center, .alignContent = ContentAlign::Center, .sideAlignContent = ContentAlign::Center })
				.small_text({ .text = "DEFENDER NAME", .color = WHITE }, &m_plateTextHandle)
				.small_text({ .text = "COST", .color = LIGHTGRAY }, &m_plateDescriptionHandle)
			.end()
		.end()
		// Defender Picker
		.custom({
			.pos = { 0, 10 },
			.draw = std::bind_front(&HUD::drawDefenders, this),
			.measure = std::bind_front(&HUD::measureDefenders, this),
			.hAlign = HAlign::Center,
            .vAlign = VAlign::Bottom
			}, &m_defenderPickerHandle)
		// Progress Bars ( HP bar )
		.custom({
			.draw = std::bind_front(&HUD::drawProgressBars, this),
		})
		// Deployed Defenders HUD
		.custom({
			.draw = std::bind_front(&HUD::drawDeployedDefenderHUD, this),
		})
		// Timeline
		.custom({
			.pos = { 0, 10 },
			.draw = std::bind_front(&HUD::drawTimeline, this),
			.measure = [timelineSideSize](const auto& size) { return timelineSideSize; },
			.hAlign = HAlign::Center,
			.vAlign = VAlign::Top,
		})
		// Message
		.custom({
			.draw = [this](auto& atlas, const auto& size){ drawMessage(atlas); }
		})
		// Fade Screen
		.custom({
			.draw = [this](const auto& atlas, const auto& size){ m_fadeScreen.draw(); }
		})
		// Skip Button
		.button({ .text = "Skip", .pos = { 10, 40 }, .size = {50, 20.f}, .onClick = [this]() { m_onSkipClickedCallbacks.executeCallbacks(); }, .hAlign = HAlign::Right, .vAlign = VAlign::Bottom}, &m_skipButtonHandle)
	.screen();
	// clang-format on
	m_screen->setVisible(false);
}

void HUD::setEnable(bool enabled) {
	m_isEnabled = enabled;
}

Color calculatePercentageColor(float value, float maxValue, Color color1 = GREEN, Color color2 = ORANGE, Color color3 = RED) {
	float t = 1.0f - value / maxValue;

	if (t < 0.5f) {
		return ColorLerp(color1, color2, t * 2.0f);
	} else {
		return ColorLerp(color2, color3, (t - 0.5f) * 2.0f);
	}
}

void HUD::update(float dt) {
	float currentBatteryPerc = m_data.batteryCharge / m_data.maxBatteryCharge;

	auto scrapsText = TextFormat("%d", m_data.scrapsAmount);
	m_screen->getText(m_scrapTextHandle).text = scrapsText;
	auto& batteryText = m_screen->getText(m_batteryTextHandle);
	batteryText.text = TextFormat("%d%%", int(currentBatteryPerc * 100));
	batteryText.color = calculatePercentageColor(m_data.batteryCharge, m_data.maxBatteryCharge);
	auto& levelName = m_screen->getText(m_levelNameHandle);
	levelName.text = m_data.levelName;

	auto& batteryIndicator = m_screen->getShape(m_batteryIndicatorHandle);
	batteryIndicator.size.y = std::round(currentBatteryPerc * maxBatteryFill);
	batteryIndicator.color = Fade(calculatePercentageColor(m_data.batteryCharge, m_data.maxBatteryCharge), 0.75f);

	auto& batteryStatIcon = m_screen->getImage(m_batteryStatIconHandle);
	batteryStatIcon.spriteIndex = static_cast<int>(std::round(currentBatteryPerc * 17.f));

	auto& plateContainer = m_screen->getBorder(m_plateContainerHandle);
	plateContainer.owner->visible = m_isAnyDefenderHovered && m_data.showPlate;
	if (m_isAnyDefenderHovered) {
		auto& defender = m_data.pickableDefenders[m_hoveredDefenderIndex];
		auto& plateText = m_screen->getText(m_plateTextHandle);
		auto& plateDescription = m_screen->getText(m_plateDescriptionHandle);

		plateText.text = defender.name;
		auto batteryGain = static_cast<int>(-defender.batteryDrain);
		const char* sign = batteryGain >= 0 ? "+" : "";
		if (defender.damage.baseDamage == 0) {
			plateDescription.text = TextFormat("Battery: %s%d", sign, batteryGain);
		} else {
			plateDescription.text = TextFormat("Battery: %s%d   Damage: %d", sign, batteryGain, (int)defender.damage.baseDamage);
		}
	}

	auto& batteryAndScrapsContainer = m_screen->getStack(m_batteryAndScrapsHandle);
	auto& defenderContainer = m_screen->getCustom(m_defenderPickerHandle);
	batteryAndScrapsContainer.owner->visible = m_data.showResources;
	defenderContainer.owner->visible = m_data.showDefenderPicker;

	for (auto& defender : m_data.pickableDefenders) {
		if (defender.isHover) {
			defender.animation.update(dt);
		}
	}

	m_data.messageTime += dt;
	m_fadeScreen.update(dt);

	m_data.batteryChargeDiff = m_data.batteryCharge - m_data.prevBatteryCharge;
	m_data.prevBatteryCharge = m_data.batteryCharge;

	auto& batteryFill = m_screen->getImage(m_batteryFillHandle);
	auto& batteryTip = m_screen->getImage(m_batteryTipHandle);
	auto& batteryBottom = m_screen->getImage(m_batteryBottomHandle);
	if (currentBatteryPerc < 0.5f && m_data.batteryChargeDiff <= 0) {
		m_blinkingBatteryTime += dt;
		auto t = (1 + sin(m_blinkingBatteryTime * (12 - currentBatteryPerc * 12))) * 0.5f;
		auto color = ColorLerp(WHITE, RED, t);
		batteryFill.color = color;
		batteryTip.color = color;
		batteryBottom.color = color;
	} else {
		m_blinkingBatteryTime = 0;
		batteryFill.color = WHITE;
		batteryTip.color = WHITE;
		batteryBottom.color = WHITE;
	}

	auto& skipButton = m_screen->getButton(m_skipButtonHandle);
	skipButton.owner->visible = m_data.showSkipButton;
}

void HUD::setVisible(bool visible) {
	m_screen->setVisible(visible);
}

CallbackHandle HUD::onDefenderSelected(std::function<void(int)> callback) {
	return m_onDefenderSelectedCallbacks.registerCallback(std::move(callback));
}

CallbackHandle HUD::onSkipClicked(std::function<void()> callback) {
	return m_onSkipClickedCallbacks.registerCallback(std::move(callback));
}

void HUD::clear() {
	m_data.pickableDefenders.clear();
	m_data.progressBars.clear();
	m_data.scrapsAmount = 0;
	m_data.batteryCharge = 0.f;
	m_data.maxBatteryCharge = 0.f;
	m_data.selectedDefenderIndex.reset();
	m_data.messageAction = {};
	m_data.showMessage = false;
	m_data.messageTime = 0;
	m_data.numberOfEnemiesDefeated = 0;
	m_data.levelName = "";
	m_data.showSkipButton = false;
	m_isAnyDefenderHovered = false;
	m_hoveredDefenderIndex = 0;
}

void HUD::startFadeInOut(std::function<void()> onFadingInDone, std::function<void()> onFadingOutDone, float seconds) {
	if (!m_fadeScreen.isFading()) {
		m_fadeScreen.startFadingInOut(std::move(onFadingInDone), std::move(onFadingOutDone), seconds);
	}
}

Vector2 HUD::measureDefenders(const Vector2& availableSize) {
	auto size = Vector2{0, defenderSlotSize};
	if (m_data.pickableDefenders.empty()) {
		return size;
	}

	for (auto& defender : m_data.pickableDefenders) {
		size.x += defenderSlotSize + defenderPadding;
	}
	size.x -= defenderPadding;

	return size;
}

void HUD::drawDefenders(Atlas& atlas, const Rectangle& bounds) {
	m_isAnyDefenderHovered = false;

	auto i = 0;
	for (auto& defender : m_data.pickableDefenders) {
		Vector2 position = {bounds.x + defenderSlotSize * i + defenderPadding * i, bounds.y};

		float halfPadding = defenderPadding / 2;
		Rectangle frameRect = {position.x - halfPadding, position.y - halfPadding, defenderSlotSize + defenderPadding, defenderSlotSize + defenderPadding};

		const bool canBuild = defender.canAfford && defender.cooldown <= 0;

		defender.isHover = false;
		bool isSelected = false;
		if (m_isEnabled) {
			isSelected = (m_data.selectedDefenderIndex == i);

			if (CheckCollisionPointRec(GetMousePosition(), frameRect)) {
				m_isAnyDefenderHovered = true;
				defender.isHover = true;
				m_hoveredDefenderIndex = i;
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
		DrawEllipse(rect.x + 7, rect.y + 3, 14, 6, defender.isHover ? Fade(WHITE, 0.25f) : Fade(BLACK, 0.25f));

		rect = LayoutHelper::arrangePositionAndSize({0, -5}, {defenderSize, defenderSize}, frameRect, HAlign::Center, VAlign::Top);
		if (defender.isHover) {
			atlas.drawSprite(defender.animation.getSpriteInfo(), {rect.x - 2, rect.y - 2}, canBuild ? defender.animation.getCurrentFrame() : 0, Flip::None, Fade(BLACK, 0.25f));
		}

		atlas.drawSprite(defender.animation.getSpriteInfo(), {rect.x, rect.y}, canBuild ? defender.animation.getCurrentFrame() : 0, Flip::None, canBuild ? WHITE : Fade({40, 40, 40, 255}, 1));

		if (defender.cooldown > 0) {
			auto progress = defender.cooldown / defender.maxCooldown;

			DrawCircleSector({position.x + defenderSlotHSize, position.y + defenderSlotHSize}, 20, -90, -90 - 360 * progress, 100, Fade(BLACK, 0.5f));

			auto cooldownText = TextFormat("%.1fs", defender.cooldown);
			rect = LayoutHelper::arrangePositionAndSize(cooldownText, 10, {0, 5}, 1, frameRect, HAlign::Center, VAlign::Top);
			DrawRectangleRec({rect.x - 2, rect.y - 2, rect.width + 4, rect.height + 2}, Fade(BLACK, 0.5f));
			DrawText(cooldownText, rect.x, rect.y, rect.height, WHITE);
		}

		const char* costText = defender.cost > 0 ? TextFormat("%d", defender.cost) : "--";
		rect = LayoutHelper::arrangePositionAndSize(costText, 10, {0, 10}, 1, frameRect, HAlign::Center, VAlign::Bottom);
		DrawRectangleRec({rect.x - 2, rect.y - 2, rect.width + 4, rect.height + 2}, Fade(BLACK, 0.5f));
		DrawText(costText, rect.x, rect.y, rect.height, defender.canAfford ? WHITE : RED);

		++i;
	}

	if (m_isAnyDefenderHovered) {
		m_gui.setCursor(CursorType::Hover);
	} else {
		m_gui.setCursor(CursorType::Default);
	}

	if (m_data.selectedDefenderIndex) {
		auto mousePos = GetMousePosition();
		auto [row, column] = getCoordinates(mousePos);
		auto position = getSnappedPosition(mousePos);

		if (row >= 0 && row < ROWS && column >= 0 && column < COLS) {
			const bool isValid = m_data.isValidBuildCellCallback(row, column);
			DrawRectangleRec({position.x, position.y, CELL_SIZE, CELL_SIZE}, isValid ? Fade(SKYBLUE, 0.5f) : Fade(RED, 0.5f));
		}

		position = Vector2Add(mousePos, {-defenderSize * 0.7f, -defenderSize * 0.7f});
		auto& defender = m_data.pickableDefenders[*m_data.selectedDefenderIndex];
		atlas.drawSprite(defender.animation.getSpriteInfo(), position, 0, None, Fade(WHITE, 0.7f));
	}
}

void HUD::drawProgressBars(Atlas& atlas, const Rectangle& bounds) {
	for (auto& progressBar : m_data.progressBars) {
		if (progressBar.value != progressBar.max) {
			drawProgressBar(progressBar.value, progressBar.max, progressBar.position, Fade(BLACK, 0.5f), calculatePercentageColor(progressBar.value, progressBar.max));
		}
	}
}

void HUD::drawProgressBar(float value, float max, const Vector2& pos, Color emptyColor, Color fillColor) {
	float barWidth = float(CELL_SIZE);
	float barHeight = 3.f;
	float valPercent = value / max;

	Vector2 barPos = {pos.x, pos.y + CELL_SIZE};
	Rectangle empty = {barPos.x, barPos.y, barWidth, barHeight};
	Rectangle fill = {barPos.x, barPos.y, barWidth * valPercent, barHeight};

	DrawRectangleRec(empty, emptyColor);
	DrawRectangleRec(fill, fillColor);
}

void HUD::drawMessage(Atlas& atlas) {
	if (m_data.showMessage) {
		auto& action = m_data.messageAction;

		float tInterpolation = 0.f;

		if (action.timer - m_data.messageTime < 0.25f) {
			auto maxInterpolation = 0.5f;
			tInterpolation = (action.timer - m_data.messageTime) / 0.25f;
		} else {
			auto maxInterpolation = 0.5f;
			tInterpolation = m_data.messageTime / maxInterpolation;
		}
		tInterpolation = std::clamp(tInterpolation, 0.f, 1.f);

		const char* text = action.text.c_str();
		auto size = MeasureTextEx(GuiGetFont(), text, action.fontSize, 1);
		auto textPos = action.textPosition.value_or(Vector2{});
		auto hAlign = action.textHAlign.value_or(HAlign::Left);
		auto vAlign = action.textVAlign.value_or(VAlign::Top);
		auto rect = LayoutHelper::arrangePositionAndSize(textPos, size, {0, 0, GAME_RENDERTEXTURE_SIZE.x, GAME_RENDERTEXTURE_SIZE.y}, hAlign, vAlign);
		auto padding = Vector2{10, 5};
		auto panelRect = Rectangle{rect.x - padding.x, rect.y - padding.y, rect.width + padding.x * 2, rect.height + padding.y * 2};
		DrawRectangleRec({panelRect.x - 2, panelRect.y - 2, panelRect.width, panelRect.height}, Fade(BLACK, 0.2f * tInterpolation)); // shadow
		DrawRectangleRec(panelRect, Fade(BLACK, 0.75f * tInterpolation));
		DrawTextEx(GuiGetFont(), text, {rect.x, rect.y}, action.fontSize, 1, Fade(WHITE, tInterpolation));

		if (m_data.messageTime >= action.timer) {
			action.timer = {};
			m_data.messageTime = 0;
			m_data.showMessage = false;
		}
	}
}

void HUD::drawDeployedDefenderHUD(Atlas& atlas, const Rectangle& bounds) {
	if (m_data.showDefenderOverlay) {
		for (auto& deployedDefender : m_data.deployedDefenders) {
			auto pos = deployedDefender.position;

			auto frameRect = Rectangle{pos.x, pos.y + 5, defenderSize, defenderSize};
			if (m_isEnabled && CheckCollisionPointRec(GetMousePosition(), frameRect)) {
				auto width = 5.f;
				auto height = 5.f;
				auto upHeight = 4.f;
				auto side = height - upHeight;
				auto bkgPadding = 1.f;

				auto alignRect = LayoutHelper::arrangePositionAndSize({-width, 0},
																	  {width + bkgPadding * 2, height * 2 + bkgPadding * 2 + 1},
																	  {pos.x, pos.y, defenderSize, defenderSize},
																	  HAlign::Left,
																	  VAlign::Center);
				pos = {alignRect.x, alignRect.y};
				auto isEnabled = deployedDefender.state != DefenderState::Off;

				DrawRectangleV(Vector2Add(pos, {-bkgPadding, -bkgPadding}), {width + bkgPadding * 2, height * 2 + bkgPadding * 2 + 1}, Fade(BLACK, 1.0f));

				constexpr bool drawSymbols = false;
				auto line = Rectangle{};
				auto circle = Rectangle{};
				if (isEnabled) {
					line = LayoutHelper::arrangePositionAndSize({}, {1.f, height / 2}, {pos.x, pos.y, width, height}, HAlign::Center, VAlign::Center);
					circle = LayoutHelper::arrangePositionAndSize({1, 0}, {height / 2, height / 2 - 2}, {pos.x, pos.y + height, width, upHeight}, HAlign::Center, VAlign::Center);

					DrawRectangleV(pos, {width, height}, DARKGREEN);

					DrawRectangleV(Vector2Add(pos, {0, height}), {width, upHeight}, GREEN);
					DrawRectangleV(Vector2Add(pos, {0, height + upHeight}), {width, 1}, LIGHTGRAY);
					DrawRectangleV(Vector2Add(pos, {0, height + upHeight + 1}), {width, side}, DARKGREEN);
				} else {
					line = LayoutHelper::arrangePositionAndSize({}, {1.f, height / 2 - 2}, {pos.x, pos.y + side + 1, width, upHeight}, HAlign::Center, VAlign::Center);
					circle = LayoutHelper::arrangePositionAndSize({1, 0}, {height / 2, height / 2}, {pos.x, pos.y + upHeight + side + 1, width, height}, HAlign::Center, VAlign::Center);

					// Define a Light Red color and a dark red color
					auto lightRed = Color{255, 100, 100, 255};
					auto darkRed = Color{150, 0, 0, 255};

					DrawRectangleV(pos, {width, side}, lightRed);
					DrawRectangleV(Vector2Add(pos, {0, side}), {width, 1}, LIGHTGRAY);
					DrawRectangleV(Vector2Add(pos, {0, side + 1}), {width, upHeight}, darkRed);

					DrawRectangleV(Vector2Add(pos, {0, side + upHeight + 1}), {width, height}, lightRed);
				}

				if constexpr (drawSymbols) {
					DrawRectangleRec(line, WHITE);
					DrawEllipseLines(circle.x + circle.width / 2, circle.y + circle.height / 2, circle.width / 2, circle.height / 2, WHITE);
				}
			}
		}
	}
}

void HUD::drawTimeline(Atlas& atlas, const Rectangle& bounds) {
	if (!m_data.showTimeline) {
		return;
	}

	auto pos = Vector2{bounds.x, bounds.y};
	auto* timelineSide = m_gui.getAtlas().getSpriteInfo("ui_timeline_side");
	auto sideSize = Vector2{static_cast<float>(timelineSide->frames[0].width), static_cast<float>(timelineSide->frames[0].height)};

	atlas.drawSprite(atlas.getSpriteInfo("ui_timeline_side"), {pos.x, pos.y}, 0);
	atlas.drawSprite(atlas.getSpriteInfo("ui_timeline_fill"), {pos.x + sideSize.x - totalTimelineFill - 2, pos.y}, {totalTimelineFill * 3 + 4, sideSize.y});
	atlas.drawSprite(atlas.getSpriteInfo("ui_timeline_side"), {pos.x + sideSize.x + totalTimelineFill, pos.y}, 0, Flip::Horizontal);

	auto padding = Vector2{10, 10};
	auto rect = Rectangle{bounds.x + padding.x, bounds.y + padding.y, bounds.width - padding.x * 2, 1};

	auto& timelineData = m_data.timelineData;

	for (auto& keyframe : timelineData.waves) {
		auto x = rect.x + keyframe.time * totalTimelineFill;
		auto y = rect.y - 2;
		atlas.drawSprite(atlas.getSpriteInfo(keyframe.icon.c_str()), {x - 8, y - 8});
	}

	auto spriteInfo = atlas.getSpriteInfo("icon_arrow_timeline");
	auto x = rect.x + (timelineData.time / timelineData.duration) * totalTimelineFill;
	auto arrowPos = Vector2{x - spriteInfo->frames->width, rect.y + 2};

	atlas.drawSprite(spriteInfo, arrowPos);
}

Vector2 HUD::measureBatteryTrend(const Vector2& availableSize) {
	return Vector2(5, 5);
}

void HUD::drawBatteryTrend(Atlas& atlas, const Rectangle& bounds) {
	auto color = GREEN;

	// Draw a triangle in raylib with a height of 10 pixels
	if (m_data.batteryChargeDiff < 0) {
		auto pointA = Vector2{bounds.x, bounds.y};
		auto pointB = Vector2{bounds.x + bounds.width, bounds.y};
		auto pointC = Vector2{bounds.x + bounds.width / 2, bounds.y + bounds.height};
		color = RED;
		DrawTriangle(pointC, pointB, pointA, color);
	} else if (m_data.batteryChargeDiff == 0) {
		DrawRectangle(bounds.x, bounds.y, bounds.width, 1, DARKGRAY);
		DrawRectangle(bounds.x, bounds.y + 3, bounds.width, 1, DARKGRAY);
	} else if (m_data.batteryChargeDiff > 0) {
		auto pointA = Vector2{bounds.x, bounds.y + bounds.height};
		auto pointB = Vector2{bounds.x + bounds.width, bounds.y + bounds.height};
		auto pointC = Vector2{bounds.x + bounds.width / 2, bounds.y};
		color = GREEN;
		DrawTriangle(pointA, pointB, pointC, color);
	}
}
