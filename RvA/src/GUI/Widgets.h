#pragma once

#include "FixedItemPool.h"

#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <optional>
#include <limits>

#include <raylib.h>

constexpr float MaxValue = std::numeric_limits<float>::max();
constexpr float autoSize = MaxValue;

using WidgetHandle = unsigned int;

enum class WidgetType
{
    Button,
    Text,
    Image,
    ProgressBar,
    Stack,
    Space,
    Shape,
    Root
};

enum class ShapeType
{
    Rectangle,
};

enum class GUIAlignmentV {
    Top,
    Bottom,
    Center
};

enum class GUIAlignmentH {
    Left,
    Right,
    Center
};

enum class GUIOrientation {
    Horizontal,
    Vertical
};

struct GUIPosition {
    // position can be treated as margins when horizontal or vertical alignments are defined
    Vector2 position;
    std::optional<GUIAlignmentH> horizontalAlignment;
    std::optional<GUIAlignmentV> verticalAlignment;
};

struct UINode
{
    WidgetHandle handle{};
    WidgetType type{};
    std::vector<std::unique_ptr<UINode>> children;
    UINode* parent{};
    Vector2 preferredSize{};
    Rectangle finalRect{};
};

struct UIStack
{
    GUIOrientation orientation{};
    Vector2 padding{};
    GUIAlignmentH horizontalAlignment{};
    GUIAlignmentV verticalAlignment{};
    Vector2 size{ MaxValue, MaxValue };
    Vector2 position{};
    WidgetHandle handle{};
};

struct UIText
{
    std::string text;
    unsigned int fontSize{};
    Color color{};
    GUIAlignmentH horizontalAlignment{};
    GUIAlignmentV verticalAlignment{};
    Vector2 position{};
    WidgetHandle handle{};
};

struct UISpace
{
    Vector2 size{};
    WidgetHandle handle{};
};

struct UIButton
{
    std::string text;
    Vector2 position{};
    Vector2 size{ MaxValue, MaxValue };
    std::function<void()> onClick{};
    GUIAlignmentH horizontalAlignment{};
    GUIAlignmentV verticalAlignment{};
    WidgetHandle handle{};
};

struct UIShape
{
    Vector2 position{};
    Vector2 size{ MaxValue, MaxValue };
    Color color{};
    ShapeType type{};
    GUIAlignmentH horizontalAlignment{};
    GUIAlignmentV verticalAlignment{};
    WidgetHandle handle{};
};

/*
* A simple class meant to create a retained UI system to represent a Menu/Screen.
* The class is meant to manage screens with few items.
*/
class Screen
{
public:
    Screen(const char* name);

    UINode& getRootNode() { return m_rootNode; }

    WidgetHandle create(UIButton button, WidgetHandle* handleResult = nullptr);
    WidgetHandle create(UIText text, WidgetHandle* handleResult = nullptr);
    WidgetHandle create(UIShape shape, WidgetHandle* handleResult = nullptr);
    WidgetHandle create(UIStack stack);
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

private:
    UINode m_rootNode;

    FixedItemPool<UIStack, 32> m_stackPool;
    FixedItemPool<UIButton, 32> m_buttonPool;
    FixedItemPool<UIText, 32> m_textPool;
    FixedItemPool<UISpace, 32> m_spacePool;
    FixedItemPool<UIShape, 32> m_shapePool;
    std::string m_name;
};

class ScreenBuilder
{
public:
    ScreenBuilder(Screen& screen);

    ScreenBuilder& stack(UIStack stack);
    ScreenBuilder& end();

    ScreenBuilder& shape(UIShape shape, WidgetHandle* handleResult = nullptr);
    ScreenBuilder& rect(const Rectangle& rect, Color color, WidgetHandle* handleResult = nullptr);
    ScreenBuilder& button(UIButton button, WidgetHandle *handleResult = nullptr);
    ScreenBuilder& text(UIText text, WidgetHandle *handleResult = nullptr);
    ScreenBuilder& space(UISpace space);

    Screen* screen() { return &m_screen; }

private:
    Screen& m_screen;
    std::stack<UINode*> m_nodeStack{};
};
