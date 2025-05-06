#include "LayoutHelper.h"

#include <raygui.h>

Vector2 LayoutHelper::adjustSize(const Vector2& preferredSize, const Vector2& availableSize) {
	return {std::min(preferredSize.x, availableSize.x), std::min(preferredSize.y, availableSize.y)};
}

Vector2 LayoutHelper::measure(UINode& node, Screen& screen, const Vector2& availableSize) {
	switch (node.type) {
	case WidgetType::Button: {
		auto& button = screen.getButton(node.handle);
		node.preferredSize = adjustSize(button.size, availableSize);
		break;
	}
	case WidgetType::Text: {
		auto& text = screen.getText(node.handle);
		node.preferredSize = MeasureTextEx(GuiGetFont(), text.text.c_str(), text.fontSize, text.fontSpacing);
		break;
	}
	case WidgetType::Space: {
		auto& space = screen.getSpace(node.handle);
		node.preferredSize = adjustSize(space.size, availableSize);
		break;
	}
	case WidgetType::Shape: {
		auto& shape = screen.getShape(node.handle);
		node.preferredSize = adjustSize(shape.size, availableSize);
		break;
	}
	case WidgetType::Stack: {
		auto& stack = screen.getStack(node.handle);
		auto stackAvailableSize = adjustSize(stack.size, availableSize);

		Vector2 preferredSize{};
		switch (stack.orientation) {
		case GUIOrientation::Horizontal:
			for (auto& child : node.children) {
				auto childSize = measure(*child, screen, stackAvailableSize);
				preferredSize.x += childSize.x + stack.padding.x;
				preferredSize.y = std::max(stackAvailableSize.y, childSize.y);
			}
			break;
		case GUIOrientation::Vertical:
			for (auto& child : node.children) {
				auto childSize = measure(*child, screen, stackAvailableSize);
				preferredSize.y += childSize.y + stack.padding.y;
				preferredSize.x = std::max(stackAvailableSize.x, childSize.x);
			}
			break;
		}
		node.preferredSize = adjustSize(availableSize, preferredSize);
		break;
	}
	case WidgetType::Border: {
		assert(node.children.size() == 1 && "Border must have one and only one child");
		auto childSize = measure(*(*node.children.begin()), screen, availableSize);
		auto& border = screen.getBorder(node.handle);
		childSize.x += 2 * border.thickness + border.padding.x;
		childSize.y += 2 * border.thickness + border.padding.y;
		node.preferredSize = adjustSize(childSize, availableSize);
		break;
	}
	case WidgetType::Custom: {
		auto& custom = screen.getCustom(node.handle);
		if (custom.measure) {
			node.preferredSize = custom.measure(availableSize);
		} else {
			node.preferredSize = availableSize;
		}
		break;
	}
	case WidgetType::Root: {
		for (auto& child : node.children) {
			measure(*child, screen, availableSize);
		}
		node.preferredSize = availableSize;
		break;
	}
	}

	return node.preferredSize;
}

void LayoutHelper::arrange(UINode& node, Screen& screen, const Rectangle& finalRect) {
	switch (node.type) {
	case WidgetType::Button: {
		auto& button = screen.getButton(node.handle);
		auto size = adjustSize(button.size, node.preferredSize);
		node.finalRect = arrangePositionAndSize(button.pos, size, finalRect, button.hAlign, button.vAlign);
		break;
	}
	case WidgetType::Text: {
		auto& text = screen.getText(node.handle);
		node.finalRect = arrangePositionAndSize(text.pos, node.preferredSize, finalRect, text.hAlign, text.vAlign);
		break;
	}
	case WidgetType::Shape: {
		auto& shape = screen.getShape(node.handle);
		auto size = adjustSize(shape.size, node.preferredSize);
		node.finalRect = arrangePositionAndSize(shape.pos, size, finalRect, shape.hAlign, shape.vAlign);
		break;
	}
	case WidgetType::Space: {
		auto& space = screen.getSpace(node.handle);
		node.finalRect = arrangePositionAndSize({0, 0}, node.preferredSize, finalRect, HAlign::Center, VAlign::Center);
		break;
	}
	case WidgetType::Border: {
		assert(node.children.size() == 1 && "Border must have one and only one child");
		auto& border = screen.getBorder(node.handle);
		node.finalRect = arrangePositionAndSize(border.pos, node.preferredSize, finalRect, border.hAlign, border.vAlign);
		auto rect = node.finalRect;
		rect.x += border.thickness + border.padding.x;
		rect.y += border.thickness + border.padding.y;
		rect.width -= 2 * (border.thickness + border.padding.x);
		rect.height -= 2 * (border.thickness + border.padding.y);
		arrange(*(*node.children.begin()), screen, rect);
		break;
	}
	case WidgetType::Custom: {
		auto& custom = screen.getCustom(node.handle);
		node.finalRect = arrangePositionAndSize(custom.pos, node.preferredSize, finalRect, custom.hAlign, custom.vAlign);
		break;
	}
	case WidgetType::Stack: {
		auto& stack = screen.getStack(node.handle);
		Rectangle stackRect = arrangePositionAndSize(stack.pos, node.preferredSize, finalRect, stack.hAlign, stack.vAlign);

		switch (stack.orientation) {
		case GUIOrientation::Horizontal: {
			auto width = 0.f;
			if (!node.children.empty()) {
				for (auto& child : node.children) {
					width += child->preferredSize.x + stack.padding.x;
				}
				width -= stack.padding.x;
			}
			auto hAlignment = toHorizontaAlignment(stack.contentAlignment);
			auto contentRect = arrangePositionAndSize({0, 0}, {width, stackRect.height}, stackRect, hAlignment, VAlign::Top);

			auto x = contentRect.x;
			for (auto& child : node.children) {
				arrange(*child, screen, {x, stackRect.y, std::min(stackRect.width, child->preferredSize.x), stackRect.height});
				x += child->finalRect.width + stack.padding.x;
			}
			break;
		}
		case GUIOrientation::Vertical: {
			auto height = 0.f;
			if (!node.children.empty()) {
				for (auto& child : node.children) {
					height += child->preferredSize.y + stack.padding.y;
				}
				height -= stack.padding.y;
			}
			auto vAlignment = toVerticalAlignment(stack.contentAlignment);
			auto contentRect = arrangePositionAndSize({0, 0}, {stackRect.width, height}, stackRect, HAlign::Left, vAlignment);
			float y = contentRect.y;
			for (auto& child : node.children) {
				arrange(*child, screen, {stackRect.x, y, stackRect.width, std::min(stackRect.height, child->preferredSize.y)});
				y += child->finalRect.height + stack.padding.y;
			}
			break;
		}
		}
		node.finalRect = stackRect;
		break;
	}
	case WidgetType::Root: {
		for (auto& child : node.children) {
			arrange(*child, screen, finalRect);
		}
		node.finalRect = finalRect;
		break;
	}
	}
}

Rectangle LayoutHelper::arrangePositionAndSize(const Vector2& position, const Vector2& size, const Rectangle& availableArea, HAlign hAlign, VAlign vAlign) {
	Rectangle rec = {position.x, position.y, size.x, size.y};

	switch (hAlign) {
	case HAlign::Left  : rec.x = availableArea.x + position.x; break;
	case HAlign::Right : rec.x = availableArea.x + availableArea.width - size.x - position.x; break;
	case HAlign::Center: rec.x = availableArea.x + (availableArea.width - size.x) / 2 + position.x; break;
	case HAlign::Stretch:
		rec.x = availableArea.x + position.x;
		rec.width = availableArea.width - position.x * 2;
		break;
	}

	switch (vAlign) {
	case VAlign::Top   : rec.y = availableArea.y + position.y; break;
	case VAlign::Bottom: rec.y = availableArea.y + availableArea.height - size.y - position.y; break;
	case VAlign::Center: rec.y = availableArea.y + (availableArea.height - size.y) / 2 + position.y; break;
	}

	return rec;
}

Rectangle LayoutHelper::arrangePositionAndSize(const char* text, float fontSize, const Vector2& position, float fontSpacing, const Rectangle& availableArea, HAlign hAlign, VAlign vAlign) {
	auto textSize = MeasureTextEx(GetFontDefault(), text, fontSize, fontSpacing);
	return arrangePositionAndSize(position, {textSize.x, textSize.y}, availableArea, hAlign, vAlign);
}

HAlign LayoutHelper::toHorizontaAlignment(ContentAlign alignment) {
	switch (alignment) {
	case ContentAlign::Start : return HAlign::Left;
	case ContentAlign::Center: return HAlign::Center;
	case ContentAlign::End	 : return HAlign::Right;
	}
	return HAlign::Left;
}

VAlign LayoutHelper::toVerticalAlignment(ContentAlign alignment) {
	switch (alignment) {
	case ContentAlign::Start : return VAlign::Top;
	case ContentAlign::Center: return VAlign::Center;
	case ContentAlign::End	 : return VAlign::Bottom;
	}
	return VAlign::Top;
}
