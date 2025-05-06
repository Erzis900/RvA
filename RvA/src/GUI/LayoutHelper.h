#pragma once

#include "GUI/Widgets.h"

class LayoutHelper {
public:
	static Vector2 adjustSize(const Vector2& preferredSize, const Vector2& availableSize);
	static Vector2 measure(UINode& node, Screen& screen, const Vector2& availableSize);
	static void arrange(UINode& node, Screen& screen, const Rectangle& finalRect);
	static Rectangle arrangePositionAndSize(const Vector2& position, const Vector2& size, const Rectangle& availableArea, HAlign hAlign, VAlign vAlign);
	static Rectangle arrangePositionAndSize(const char* text, float fontSize, const Vector2& position, float fontSpacing, const Rectangle& availableArea, HAlign hAlign, VAlign vAlign);

	static HAlign toHorizontaAlignment(ContentAlign alignment);
	static VAlign toVerticalAlignment(ContentAlign alignment);
};
