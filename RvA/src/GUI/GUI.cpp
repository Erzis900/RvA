#include "GUI.h"

#include "LayoutHelper.h"
#include "MusicManager.h"
#include "constants.h"

#include <gui/GUIStyle.h>
#include <raymath.h>

GUI::GUI(Atlas& atlas, MusicManager& musicManager) : m_atlas(atlas), m_musicManager(musicManager) {}

GUI::~GUI() {
	UnloadFont(m_font);
}

void GUI::loadResources() {
	GuiLoadStyleRva();
	m_font = LoadFont("assets/fonts/mecha.png");
	GuiSetFont(m_font);
	GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SMALL);

	// This is not a real load of resources but it's a way to retrieve and store the SpriteInfo
	m_mousePointSprite = m_atlas.getSpriteInfo("mouse_cursor_point");
	m_mouseHoverSprite = m_atlas.getSpriteInfo("mouse_cursor_hover");
}

void GUI::update(float dt) {
	m_fadeScreen.update(dt);
	for (auto& [id, screen] : m_screens) {
		screen->getImages().forEachActive([&](auto handle, UIImg& image) {
			if (std::holds_alternative<Animation>(image.sprite)) {
				auto& animation = std::get<Animation>(image.sprite);
				animation.update(dt);
			}
		});
	}
}

void GUI::draw() {
	drawScreens();
	drawFPS();
	drawCursor();
	m_fadeScreen.draw();
}

void GUI::drawCursor() {
	m_atlas.drawSprite(m_mouseCurrentSprite, {GetMousePosition().x - 10, GetMousePosition().y - 5}, {32, 32});
}

void GUI::drawFPS() {
	auto fpsText = TextFormat("%d", GetFPS());
	auto rect = LayoutHelper::arrangePositionAndSize(fpsText, 10, {10, 10}, 1, {0, 0, UI_RENDERTEXTURE_SIZE.x, UI_RENDERTEXTURE_SIZE.y}, HAlign::Right, VAlign::Bottom);
	::DrawText(fpsText, static_cast<int>(rect.x), static_cast<int>(rect.y), rect.height, GREEN);
}

void GUI::drawScreens() {
	for (const auto& name : m_screensToDestroy) {
		auto it = std::ranges::find_if(m_screens, [&name](const auto& pair) { return pair.first == name; });
		if (it != m_screens.end()) {
			m_screens.erase(it);
		}
	}
	m_screensToDestroy.clear();

	m_drawingScreens = true;
	for (auto& [name, screen] : m_screens) {
		if (!screen->isVisible()) {
			continue;
		}
		auto& root = screen->getRootNode();

		LayoutHelper::measure(root, *screen, {UI_RENDERTEXTURE_SIZE.x, UI_RENDERTEXTURE_SIZE.y});
		LayoutHelper::arrange(root, *screen, {0, 0, UI_RENDERTEXTURE_SIZE.x, UI_RENDERTEXTURE_SIZE.y});
		drawWidget(root, *screen);
	}
	m_drawingScreens = false;
}

void GUI::drawWidget(UINode& node, Screen& screen) {
	if (!node.visible) {
		return;
	}

	switch (node.type) {
	case WidgetType::Button: {
		auto& button = screen.getButton(node.handle);
		bool isPressed = false;
		if (button.useLabelStyle) {
			isPressed = ::GuiLabelButton(node.finalRect, button.text.c_str());
		} else {
			isPressed = ::GuiButton(node.finalRect, button.text.c_str());
		}
		if (isPressed) {
			m_musicManager.playSound("button_click");
			button.onClick();
		}
		break;
	}
	case WidgetType::Text: {
		auto& text = screen.getText(node.handle);
		::DrawTextEx(m_font, text.text.c_str(), {node.finalRect.x, node.finalRect.y}, node.finalRect.height, text.fontSpacing, text.color);
		break;
	}
	case WidgetType::Custom: {
		auto& custom = screen.getCustom(node.handle);
		custom.draw(m_atlas, node.finalRect);
		break;
	}
	case WidgetType::Shape: {
		auto& shape = screen.getShape(node.handle);
		switch (shape.type) {
		case ShapeType::Rectangle: {
			if (shape.roundness > 0) {
				::DrawRectangleRounded(node.finalRect, shape.roundness, 0, shape.color);
			} else {
				::DrawRectangleRec(node.finalRect, shape.color);
			}
			break;
		}
		}
		break;
	}
	case WidgetType::Image: {
		auto& image = screen.getImage(node.handle);
		std::visit(
			[&](auto&& arg) {
				auto spriteIndex = image.spriteIndex.value_or(0);
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, const SpriteInfo*>) {
					switch (image.textureFillMode) {
					case TextureFillMode::Repeat: {
						// Not exactly the right repeat approach. Must be improved as it doesn't take care of the calculated size.
						for (int y = 0; y < node.finalRect.height; y += arg->frames[0].height) {
							for (int x = 0; x < node.finalRect.width; x += arg->frames[0].width) {
								m_atlas
									.drawSprite(arg, {node.finalRect.x + x, node.finalRect.y + y}, {(float)arg->frames[0].width, (float)arg->frames[0].height}, spriteIndex, image.flip, image.color);
							}
						}
						break;
					}
					case TextureFillMode::Stretch: {
						m_atlas.drawSprite(arg, {node.finalRect.x, node.finalRect.y}, {node.finalRect.width, node.finalRect.height}, spriteIndex, image.flip, image.color);
						break;
					}
					}
				} else if constexpr (std::is_same_v<T, Animation>) {
					Animation& animation = arg;
					auto frame = animation.getSpriteInfo()->frames;
					m_atlas.drawSprite(animation.getSpriteInfo(),
									   {node.finalRect.x, node.finalRect.y},
									   {node.finalRect.width, node.finalRect.height},
									   animation.getCurrentFrame(),
									   image.flip,
									   image.color);
				} else {
					static_assert(sizeof(T) == 0, "Non-exhaustive visitor!");
				}
			},
			image.sprite);
		break;
	}
	case WidgetType::Border: {
		auto& border = screen.getBorder(node.handle);

		std::visit(
			[&](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, std::pair<Color, Color>>) {
					::DrawRectangleGradientEx(node.finalRect, arg.first, arg.first, arg.second, arg.second);
				} else if constexpr (std::is_same_v<T, Color>) {
					::DrawRectangleRec(node.finalRect, arg);
				} else {
					static_assert(sizeof(T) == 0, "Non-exhaustive visitor!");
				}
			},
			border.bkgColor);

		::DrawRectangleLinesEx(node.finalRect, border.thickness, border.color);
		break;
	}
	}

	if (m_isDebugViewEnabled) {
		if (node.type == WidgetType::Space) {
			auto& rect = node.finalRect;
			::DrawLineEx({rect.x, rect.y}, {rect.x + rect.width, rect.y + rect.height}, 2, BLUE);
		} else {
			::DrawRectangleLinesEx(node.finalRect, 1, RED);
		}
	}

	for (auto& child : node.children) {
		drawWidget(*child, screen);
	}
}

void GUI::setCursor(CursorType type) {
	switch (type) {
	case CursorType::Default: m_mouseCurrentSprite = m_mousePointSprite; break;
	case CursorType::Hover	: m_mouseCurrentSprite = m_mouseHoverSprite; break;
	}
}

void GUI::startFadingInOut(std::function<void()> onFadingInDone, std::function<void()> onFadingOutDone, float seconds) {
	m_fadeScreen.startFadingInOut(std::move(onFadingInDone), std::move(onFadingOutDone), seconds);
}

void GUI::destroyScreen(const char* name) {
	auto it = std::ranges::find_if(m_screens, [&name](const auto& pair) { return pair.first == name; });
	if (it != m_screens.end()) {
		if (m_drawingScreens) {
			m_screensToDestroy.push_back(name);
		} else {
			m_screens.erase(it);
		}
	}
}

ScreenBuilder GUI::buildScreen(const char* name) {
	auto screen = std::make_unique<Screen>(name);
	auto screenPtr = screen.get();
	m_screens.emplace_back(name, std::move(screen));
	return ScreenBuilder(*screenPtr);
}

void GUI::toggleDebugView() {
	m_isDebugViewEnabled = !m_isDebugViewEnabled;
}
