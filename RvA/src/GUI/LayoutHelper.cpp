#include "LayoutHelper.h"

Vector2 LayoutHelper::adjustSize(const Vector2& preferredSize, const Vector2& availableSize)
{
    return {
        std::min(preferredSize.x, availableSize.x),
        std::min(preferredSize.y, availableSize.y)
    };
}

Vector2 LayoutHelper::measure(UINode& node, Screen& screen, const Vector2& availableSize) {
    switch (node.type) {
    case WidgetType::Button:
    {
        auto &button = screen.getButton(node.handle);
        node.preferredSize = adjustSize(button.size, availableSize);
        break;
    }
    case WidgetType::Text:
    {
        auto &text = screen.getText(node.handle);
        node.preferredSize = { static_cast<float>(MeasureText(text.text.c_str(), text.fontSize)), static_cast<float>(text.fontSize) };
        break;
    }
    case WidgetType::Space:
    {
        auto &space = screen.getSpace(node.handle);
        node.preferredSize = adjustSize(space.size, availableSize);
        break;
    }
    case WidgetType::Shape:
    {
        auto& shape = screen.getShape(node.handle);
        node.preferredSize = adjustSize(shape.size, availableSize);
        break;
    }
    case WidgetType::Stack:
    {
        auto &stack = screen.getStack(node.handle);
        auto stackAvailableSize = adjustSize(stack.size, availableSize);

        Vector2 preferredSize{};
        switch (stack.orientation) {
        case GUIOrientation::Horizontal:
            for (auto &child : node.children) {
                auto childSize = measure(*child, screen, { MaxValue, stackAvailableSize.y });
                preferredSize.x += childSize.x + stack.padding.x;
                preferredSize.y = std::max(stackAvailableSize.y, childSize.y);
            }
            break;
        case GUIOrientation::Vertical:
            for (auto &child : node.children) {
                auto childSize = measure(*child, screen, { stackAvailableSize.x, MaxValue });
                preferredSize.y += childSize.y + stack.padding.y;
                preferredSize.x = std::max(stackAvailableSize.x, childSize.x);
            }
            break;
        }
        node.preferredSize = adjustSize(availableSize, preferredSize);
        break;
    }
    case WidgetType::Root:
    {
        for (auto &child : node.children) {
            measure(*child, screen, availableSize);
        }
        break;
    }
    }

    return node.preferredSize;
}

void LayoutHelper::arrange(UINode& node, Screen& screen, const Rectangle& finalRect)
{
    switch (node.type) {
    case WidgetType::Button:
    {
        auto &button = screen.getButton(node.handle);
        auto size = adjustSize(button.size, node.preferredSize);
        node.finalRect = arrangePositionAndSize(button.position, size, finalRect, button.horizontalAlignment, button.verticalAlignment);
        break;
    }
    case WidgetType::Text:
    {
        auto &text = screen.getText(node.handle);
        node.finalRect = arrangePositionAndSize(text.position, node.preferredSize, finalRect, text.horizontalAlignment, text.verticalAlignment);
        break;
    }
    case WidgetType::Shape:
    {
        auto& shape = screen.getShape(node.handle);
        auto size = adjustSize(shape.size, node.preferredSize);
        node.finalRect = arrangePositionAndSize(shape.position, size, finalRect, shape.horizontalAlignment, shape.verticalAlignment);
        break;
    }
    case WidgetType::Space:
    {
        auto &space = screen.getSpace(node.handle);
        node.finalRect = arrangePositionAndSize({ 0, 0 }, node.preferredSize, finalRect, {}, {});
        break;
    }
    case WidgetType::Stack:
    {
        auto &stack = screen.getStack(node.handle);
        Rectangle stackRect = arrangePositionAndSize(stack.position, node.preferredSize, finalRect, stack.horizontalAlignment, stack.verticalAlignment);
        switch (stack.orientation) {
        case GUIOrientation::Horizontal: {
            float x = stackRect.x;
            for (auto &child : node.children) {
                arrange(*child, screen, { x, stackRect.y, stackRect.width, stackRect.height });
                x += child->finalRect.width + stack.padding.x;
            }
            break;
        }
        case GUIOrientation::Vertical: {
            float y = stackRect.y;
            for (auto &child : node.children) {
                arrange(*child, screen, { stackRect.x, y, stackRect.width, stackRect.height });
                y += child->finalRect.height + stack.padding.y;
            }
            break;
        }
        }
        break;
    }
    case WidgetType::Root:
    {
        for (auto &child : node.children) {
            arrange(*child, screen, finalRect);
        }
        break;
    }
    }
}

Rectangle LayoutHelper::arrangePositionAndSize(const Vector2& position, const Vector2& size, const Rectangle& availableArea, GUIAlignmentH horizontalAlignment, GUIAlignmentV verticalAlignment)
{
    Vector2 result = position;

    switch (horizontalAlignment) {
    case GUIAlignmentH::Left:
        result.x = availableArea.x + position.x;
        break;
    case GUIAlignmentH::Right:
        result.x = availableArea.x + availableArea.width - size.x - position.x;
        break;
    case GUIAlignmentH::Center:
        result.x = availableArea.x + (availableArea.width - size.x) / 2 + position.x;
        break;
    }

    switch (verticalAlignment) {
    case GUIAlignmentV::Top:
        result.y = availableArea.y + position.y;
        break;
    case GUIAlignmentV::Bottom:
        result.y = availableArea.y + availableArea.height - size.y - position.y;
        break;
    case GUIAlignmentV::Center:
        result.y = availableArea.y + (availableArea.height - size.y) / 2 + position.y;
        break;
    }

    return { result.x, result.y, size.x, size.y };
}

Rectangle LayoutHelper::arrangePositionAndSize(const char* text, float fontSize, const Vector2& position, const Rectangle& availableArea, GUIAlignmentH horizontalAlignment, GUIAlignmentV verticalAlignment) {
    auto textSize = MeasureTextEx(GetFontDefault(), text, fontSize, 1);
    return arrangePositionAndSize(position, { textSize.x, textSize.y }, availableArea, horizontalAlignment, verticalAlignment);
}
