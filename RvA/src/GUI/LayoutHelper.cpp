#include "LayoutHelper.h"

#include <raygui.h>

Vector2 LayoutHelper::adjustSize(const Vector2& preferredSize, const Vector2& availableSize, Fit fit) {
	if (fit == Fit::Fill) {
		return {std::min(preferredSize.x, availableSize.x), std::min(preferredSize.y, availableSize.y)};
	} else if (fit == Fit::Contain) {
		// keep the aspect ratio of the icon by trying to fit the availableSize
		float aspectRatio = preferredSize.x / preferredSize.y;
		float newWidth = availableSize.x;
		float newHeight = availableSize.y;
		if (availableSize.x > availableSize.y) {
			newWidth = availableSize.y * aspectRatio;
		} else {
			newHeight = availableSize.x / aspectRatio;
		}
		return {newWidth, newHeight};
	} else if (fit == Fit::Ignore) {
		return preferredSize;
	} else {
		assert(false && "Unknown fit type");
		return {0, 0};
	}
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
		node.preferredSize = MeasureTextEx(GuiGetFont(), text.text.c_str(), static_cast<float>(text.fontSize), text.fontSpacing);
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
	case WidgetType::Slider: {
		auto& slider = screen.getSlider(node.handle);
		node.preferredSize = adjustSize(slider.size, availableSize);
		break;
	}
	case WidgetType::Image: {
		auto& image = screen.getImage(node.handle);
		auto spriteSize = std::visit(
			[](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, const SpriteInfo*>) {
					texture_atlas_frame_t* frameInfo = arg->frames; // take the first frame
					return Vector2{static_cast<float>(frameInfo->width), static_cast<float>(frameInfo->height)};
				} else if constexpr (std::is_same_v<T, Animation>) {
					Animation& animation = arg;
					texture_atlas_frame_t* frameInfo = animation.getSpriteInfo()->frames; // take the first frame
					return Vector2{static_cast<float>(frameInfo->width), static_cast<float>(frameInfo->height)};
				} else {
					static_assert(sizeof(T) == 0, "Non-exhaustive visitor!");
				}
			},
			image.sprite);
		auto size = image.size ? *image.size : spriteSize;
		node.preferredSize = adjustSize(size, availableSize, image.fit);
		break;
	}
	case WidgetType::Stack: {
		auto& stack = screen.getStack(node.handle);
		auto stackAvailableSize = Vector2{};
		if (!stack.size.has_value()) {
			stackAvailableSize = availableSize;
		} else {
			stackAvailableSize = adjustSize(*stack.size, availableSize);
		}

		Vector2 preferredSize{};
		switch (stack.orientation) {
		case GUIOrientation::Horizontal:
			for (auto& child : node.children) {
				auto childSize = measure(*child, screen, stackAvailableSize);
				preferredSize.x += childSize.x + stack.padding.x;
				preferredSize.y = std::max(preferredSize.y, childSize.y);
			}
			break;
		case GUIOrientation::Vertical:
			for (auto& child : node.children) {
				auto childSize = measure(*child, screen, stackAvailableSize);
				preferredSize.y += childSize.y + stack.padding.y;
				preferredSize.x = std::max(preferredSize.x, childSize.x);
			}
			break;
		}

		if (!stack.size.has_value()) {
			node.preferredSize = adjustSize(preferredSize, stackAvailableSize);
		} else {
			node.preferredSize = adjustSize(*stack.size, availableSize);
		}
		break;
	}
	case WidgetType::Border: {
		assert(node.children.size() == 1 && "Border must have one and only one child");
		auto finalSize = measure(*(*node.children.begin()), screen, availableSize);
		auto& border = screen.getBorder(node.handle);
		finalSize.x += 2 * (border.thickness + border.padding.x);
		finalSize.y += 2 * (border.thickness + border.padding.y);
		finalSize.x = std::max(finalSize.x, border.size.x);
		finalSize.y = std::max(finalSize.y, border.size.y);
		node.preferredSize = adjustSize(finalSize, availableSize);
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
	case WidgetType::Slider: {
		auto& slider = screen.getSlider(node.handle);
		auto size = adjustSize(slider.size, node.preferredSize);
		node.finalRect = arrangePositionAndSize(slider.pos, size, finalRect, slider.hAlign, slider.vAlign);
		break;
	}
	case WidgetType::Image: {
		auto& image = screen.getImage(node.handle);
		node.finalRect = arrangePositionAndSize(image.pos, node.preferredSize, finalRect, image.hAlign, image.vAlign);
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
			auto height = 0.f;
			if (!node.children.empty()) {
				for (auto& child : node.children) {
					width += child->preferredSize.x + stack.padding.x;
					height = std::max(height, child->preferredSize.y);
				}
				width -= stack.padding.x;
			}
			auto hAlignment = toHorizontaAlignment(stack.alignContent);
			auto vAlignment = toVerticalAlignment(stack.sideAlignContent);
			// Calculate rect where we should position the items
			auto contentRect = arrangePositionAndSize({0, 0}, {width, height}, stackRect, hAlignment, vAlignment);
			auto x = contentRect.x;
			for (auto& child : node.children) {
				// Calculate correct area for this children
				auto childRect =
					arrangePositionAndSize({0, 0}, child->preferredSize, {x, contentRect.y, std::min(stackRect.width, child->preferredSize.x), contentRect.height}, HAlign::Left, vAlignment);

				arrange(*child, screen, childRect);
				x += child->finalRect.width + stack.padding.x;
			}
			break;
		}
		case GUIOrientation::Vertical: {
			auto height = 0.f;
			auto width = 0.f;
			if (!node.children.empty()) {
				for (auto& child : node.children) {
					height += child->preferredSize.y + stack.padding.y;
					width = std::max(width, child->preferredSize.x);
				}
				height -= stack.padding.y;
			}
			auto vAlignment = toVerticalAlignment(stack.alignContent);
			auto hAlignment = toHorizontaAlignment(stack.sideAlignContent);
			auto contentRect = arrangePositionAndSize({0, 0}, {width, height}, stackRect, hAlignment, vAlignment);
			float y = contentRect.y;
			for (auto& child : node.children) {
				// Calculate correct area for this children
				auto childRect =
					arrangePositionAndSize({0, 0}, child->preferredSize, {contentRect.x, y, contentRect.width, std::min(stackRect.height, child->preferredSize.y)}, hAlignment, VAlign::Top);
				arrange(*child, screen, childRect);
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
	case VAlign::Stretch:
		rec.y = availableArea.y + position.y;
		rec.height = availableArea.height - position.y * 2;
		break;
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
