#pragma once

#include "FixedItemPool.h"
#include "atlas/Atlas.h"

#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <raylib.h>
#include <string>
#include <variant>
#include <vector>

constexpr float MaxValue = std::numeric_limits<float>::max();
constexpr float autoSize = MaxValue;

using WidgetHandle = unsigned int;
class Atlas;

enum class WidgetType {
	Button,
	Text,
	Image,
	Stack,
	Space,
	Custom,
	Shape,
	Border,
	Root
};

enum class ShapeType {
	Rectangle,
};

enum class VAlign {
	Top,
	Bottom,
	Center,
	Stretch
};

enum class HAlign {
	Left,
	Right,
	Center,
	Stretch
};

enum class ContentAlign {
	Start,
	Center,
	End
};

enum class GUIOrientation {
	Horizontal,
	Vertical
};

enum class Fit {
	Fill,
	Contain,
	Ignore
};

enum class TextureFillMode {
	Stretch,
	Repeat
};

struct UINode {
	WidgetHandle handle{};
	WidgetType type{};
	std::vector<std::unique_ptr<UINode>> children;
	UINode* parent{};
	Vector2 preferredSize{};
	Rectangle finalRect{};
	bool visible{true};
};

struct UIStack {
	GUIOrientation orientation{};
	Vector2 padding{};
	HAlign hAlign{};
	VAlign vAlign{};
	Vector2 size{MaxValue, MaxValue};
	Vector2 pos{};
	ContentAlign alignContent{};
	ContentAlign sideAlignContent{};
	WidgetHandle handle{};
	UINode* owner{};
};

struct UIText {
	std::string text;
	unsigned int fontSize{};
	Color color{};
	HAlign hAlign{};
	VAlign vAlign{};
	Vector2 pos{};
	float fontSpacing{1};
	WidgetHandle handle{};
	UINode* owner{};
};

struct UISpace {
	Vector2 size{};
	WidgetHandle handle{};
	UINode* owner{};
};

using UIColorEx = std::variant<Color, std::pair<Color, Color>>;

struct UIBorder {
	Color color{};
	UIColorEx bkgColor{Fade(BLACK, 0)};
	float thickness{1};
	Vector2 pos{};
	Vector2 size{};
	Vector2 padding{};
	HAlign hAlign{};
	VAlign vAlign{};
	WidgetHandle handle{};
	UINode* owner{};
};

struct UIButton {
	std::string text;
	Vector2 pos{};
	Vector2 size{MaxValue, MaxValue};
	std::function<void()> onClick{};
	HAlign hAlign{};
	VAlign vAlign{};
	bool useLabelStyle{};
	WidgetHandle handle{};
	UINode* owner{};
};

struct UIShape {
	Vector2 pos{};
	Vector2 size{MaxValue, MaxValue};
	Color color{};
	ShapeType type{};
	float roundness{};
	HAlign hAlign{};
	VAlign vAlign{};
	WidgetHandle handle{};
	UINode* owner{};
};

struct UIImg {
	Vector2 pos{};
	std::optional<Vector2> size{};
	const SpriteInfo* sprite{};
	HAlign hAlign{};
	VAlign vAlign{};
	Fit fit{};
	Flip flip{};
	TextureFillMode textureFillMode;
	WidgetHandle handle{};
	UINode* owner{};
};

struct UICustom {
	Vector2 pos{};
	std::function<void(Atlas&, const Rectangle& bounds)> draw{};
	std::function<Vector2(const Vector2&)> measure{};
	HAlign hAlign{};
	VAlign vAlign{};
	WidgetHandle handle{};
	UINode* owner{};
};

/*
 * A simple class meant to create a retained UI system to represent a Menu/Screen.
 * The class is meant to manage screens with few items.
 */
class Screen {
public:
	Screen(const char* name);

	UINode& getRootNode() {
		return m_rootNode;
	}

	WidgetHandle create(UIButton button, WidgetHandle* handleResult = nullptr);
	WidgetHandle create(UIText text, WidgetHandle* handleResult = nullptr);
	WidgetHandle create(UIShape shape, WidgetHandle* handleResult = nullptr);
	WidgetHandle create(UIBorder border, WidgetHandle* handleResult = nullptr);
	WidgetHandle create(UIImg image, WidgetHandle* handleResult = nullptr);
	WidgetHandle create(UICustom custom, WidgetHandle* handleResult = nullptr);
	WidgetHandle create(UIStack stack, WidgetHandle* handleResult = nullptr);
	WidgetHandle create(UISpace space);

	UIButton& getButton(WidgetHandle handle) {
		return *m_buttonPool.getItem(handle);
	}

	UIText& getText(WidgetHandle handle) {
		return *m_textPool.getItem(handle);
	}

	UIStack& getStack(WidgetHandle handle) {
		return *m_stackPool.getItem(handle);
	}

	UISpace& getSpace(WidgetHandle handle) {
		return *m_spacePool.getItem(handle);
	}

	UIShape& getShape(WidgetHandle handle) {
		return *m_shapePool.getItem(handle);
	}

	UICustom& getCustom(WidgetHandle handle) {
		return *m_customPool.getItem(handle);
	}

	UIBorder& getBorder(WidgetHandle handle) {
		return *m_borderPool.getItem(handle);
	}

	UIImg& getImage(WidgetHandle handle) {
		return *m_imagePool.getItem(handle);
	}

	void setVisible(bool visible) {
		m_isVisible = visible;
	}

	[[nodiscard]] bool isVisible() const {
		return m_isVisible;
	}

private:
	UINode m_rootNode;

	// Not that good to have fixed item pool this way but it works fine for now.
	FixedItemPool<UIStack, 32> m_stackPool;
	FixedItemPool<UIButton, 32> m_buttonPool;
	FixedItemPool<UIText, 32> m_textPool;
	FixedItemPool<UISpace, 32> m_spacePool;
	FixedItemPool<UIShape, 32> m_shapePool;
	FixedItemPool<UICustom, 32> m_customPool;
	FixedItemPool<UIBorder, 32> m_borderPool;
	FixedItemPool<UIImg, 32> m_imagePool;
	std::string m_name;
	bool m_isVisible{true};
};

class ScreenBuilder {
public:
	ScreenBuilder(Screen& screen);

	ScreenBuilder& stack(UIStack stack, WidgetHandle* handleResult = nullptr);
	ScreenBuilder& vertical_stack(float padding, float size, VAlign vAlignment = VAlign::Center, HAlign hAlignment = HAlign::Center, ContentAlign cAlignment = ContentAlign::Center);
	ScreenBuilder& horizontal_stack(float padding, float size, HAlign hAlignment = HAlign::Center, VAlign vAlignment = VAlign::Center, ContentAlign cAlignment = ContentAlign::Center);
	ScreenBuilder& end();

	ScreenBuilder& shape(UIShape shape, WidgetHandle* handleResult = nullptr);
	ScreenBuilder& rect(const Rectangle& rect, Color color, WidgetHandle* handleResult = nullptr);
	ScreenBuilder& button(UIButton button, WidgetHandle* handleResult = nullptr);
	ScreenBuilder& label_button(UIButton button, WidgetHandle* handleResult = nullptr);
	ScreenBuilder& text(UIText text, WidgetHandle* handleResult = nullptr);
	ScreenBuilder& small_text(UIText text, WidgetHandle* handleResult = nullptr);
	ScreenBuilder& medium_text(UIText text, WidgetHandle* handleResult = nullptr);
	ScreenBuilder& big_text(UIText text, WidgetHandle* handleResult = nullptr);
	ScreenBuilder& border(UIBorder border, WidgetHandle* handleResult = nullptr);
	ScreenBuilder& image(UIImg image, WidgetHandle* handleResult = nullptr);
	ScreenBuilder& custom(UICustom custom, WidgetHandle* handleResult = nullptr);
	ScreenBuilder& space(UISpace space);
	ScreenBuilder& default_bkg(float alpha = 1.f);

	Screen* screen() {
		return &m_screen;
	}

private:
	Screen& m_screen;
	std::stack<UINode*> m_nodeStack{};
};
