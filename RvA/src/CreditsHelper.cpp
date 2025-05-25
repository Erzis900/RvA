#include "CreditsHelper.h"

#include "GUI/GUI.h"

#include <raylib.h>
#include <string>
#include <vector>

struct CreditsItem {
	std::string label;
	Color color{WHITE};
	bool isTitle{};
	std::string spriteName{};
	Color bkgColor{BLACK};
	Vector2 iconSize;
};

auto titleColor = WHITE;
auto nameColor = Color{140, 140, 140, 255};

// TODO(Gerark) - This list could be defined in data ( maybe a json file to easily edit it without recompiling )
const std::vector<CreditsItem> creditsItems{
	{"PROGRAMMING", titleColor, true, "scrap_idle", DARKGRAY, {32, 32}},
	{"Gerark", nameColor},
	{"Erzis", nameColor},
	{"Zazeraz", nameColor},
	{},
	{"PIXEL ART", titleColor, true, "solar_idle", DARKGREEN, {32, 32}},
	{"Marvin Gorner", nameColor},
	{"Adsol", nameColor},
	{"Cabbage", nameColor},
	{},
	{"CONCEPT ART", titleColor, true, "shooter_idle", Fade(RED, 0.5f), {32, 32}},
	{"yeahno", nameColor},
	{},
	{"MUSIC", titleColor, true, "audio_enabled", PINK, {50, 50}},
	{"Trimi3je", nameColor},
	{"Frog Wizard", nameColor},
};

void CreditsHelper::fillCredits(ScreenBuilder& builder, GUI& gui) {
	builder.stack({.orientation = GUIOrientation::Vertical,
				   .padding = {0, 0},
				   .hAlign = HAlign::Right,
				   .vAlign = VAlign::Top,
				   .size = Vec2{200, autoSize},
				   .pos = {10, 10},
				   .sideAlignContent = ContentAlign::End});

	// clang-format off
	for (auto& item : creditsItems) {
		if (!item.label.empty()) {
			if (item.isTitle) {
					builder.border({.bkgColor = std::make_pair(Fade(item.bkgColor, 0), Fade(item.bkgColor, 0.6)), .padding = {5, 0}, .hAlign = HAlign::Right});
						builder.stack({.orientation = GUIOrientation::Horizontal, .padding = {10, 0}, .hAlign = HAlign::Right, .vAlign = VAlign::Top, .size = Vec2{150, 32}, .pos = {0, 0}, .alignContent = ContentAlign::End, .sideAlignContent = ContentAlign::Center});
							builder.text({.text = item.label.c_str(), .fontSize = 24, .color = item.color, .hAlign = HAlign::Right});
							builder.image({.pos = {0, 0}, .size = item.iconSize, .sprite = Animation::createAnimation({ gui.getAtlas().getSpriteInfo(item.spriteName.c_str()), 0.1f }), .fit = Fit::Contain });
						builder.end();
					builder.end();
			} else {
				builder.border({.color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0), Fade(BLACK, 0.5)), .size = {100, 15}, .padding = {5, 0}, .hAlign = HAlign::Right});
					builder.small_text({.text = item.label.c_str(), .color = item.color, .hAlign = HAlign::Right});
				builder.end();
			}
		} else {
			builder.space({0, 10.f});
		}
	}
	builder.end();
	// clang-format on
}
