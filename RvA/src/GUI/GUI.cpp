#include "GUI.h"

#include <raygui.h>
#include <raymath.h>

#include "LayoutHelper.h"
#include "constants.h"

GUI::GUI(Atlas& atlas) : m_atlas(atlas)
{
}

void GUI::loadResources()
{
	// This is not a real load of resources but it's a way to retrieve and store the SpriteInfo
	m_mousePointSprite = m_atlas.getSpriteInfo("mouse_cursor_point");
	m_mouseHoverSprite = m_atlas.getSpriteInfo("mouse_cursor_hover");
}

void GUI::update(float dt)
{
	m_fading.update(dt);
}

void GUI::draw()
{
    drawScreens();
    drawFPS();
    drawCursor();
	drawFading();
}

void GUI::drawCursor()
{
	m_atlas.drawSprite(m_mouseCurrentSprite, { GetMousePosition().x - 10, GetMousePosition().y - 5 });
}

void GUI::drawFPS()
{
	auto fpsText = std::to_string(GetFPS());
	auto rect = LayoutHelper::arrangePositionAndSize(fpsText.c_str(), 10, { 10, 10 }, {0, 0, TEX_WIDTH, TEX_HEIGHT }, GUIAlignmentH::Right, GUIAlignmentV::Bottom);
	::DrawText(fpsText.c_str(), static_cast<int>(rect.x), static_cast<int>(rect.y), rect.height, GREEN);
}

void GUI::drawScreens()
{
    for (const auto& name : m_screensToDestroy) {
        auto it = m_screens.find(name);
        if (it != m_screens.end()) {
            m_screens.erase(it);
        }
    }
	m_screensToDestroy.clear();

	m_drawingScreens = true;
	for (auto &[name, screen] : m_screens) {
		auto &root = screen->getRootNode();

        LayoutHelper::measure(root, *screen, { TEX_WIDTH, TEX_HEIGHT });
        LayoutHelper::arrange(root, *screen, { 0, 0, TEX_WIDTH, TEX_HEIGHT });
		drawWidget(root, *screen);
	}
	m_drawingScreens = false;
}

void GUI::drawWidget(UINode &node, Screen &screen) {
	switch (node.type) {
	case WidgetType::Button: {
		auto& button = screen.getButton(node.handle);
		if (::GuiButton(node.finalRect, button.text.c_str())) {
			button.onClick();
		}
		break;
	}
	case WidgetType::Text: {
		auto& text = screen.getText(node.handle);
		::DrawText(text.text.c_str(), node.finalRect.x, node.finalRect.y, text.fontSize, text.color);
		break;
	}
	case WidgetType::Shape: {
		auto& shape = screen.getShape(node.handle);
		switch (shape.type) {
		case ShapeType::Rectangle:
			::DrawRectangleRec(node.finalRect, shape.color);
			break;
		}
		break;
	}
	}

	for (auto &child : node.children)
	{
        drawWidget(*child, screen);
	}
}

void GUI::drawFading()
{
	DrawRectangle(0, 0, TEX_WIDTH, TEX_HEIGHT, m_fading.getValue());
}

void GUI::setCursor(CursorType type)
{
	switch (type)
	{
	case CursorType::Point:
		m_mouseCurrentSprite = m_mousePointSprite;
		break;
	case CursorType::Hover:
		m_mouseCurrentSprite = m_mouseHoverSprite;
		break;
	}
}

bool GUI::drawButton(DrawButtonInfo drawButtonInfo)
{
	auto position = calculateCoordinates(drawButtonInfo);
	return ::GuiButton({
		.x = position.x,
		.y = position.y,
		.width = drawButtonInfo.size.x,
		.height = drawButtonInfo.size.y
		}, drawButtonInfo.text);
}

void GUI::drawText(DrawTextInfo drawTextInfo)
{
	auto position = calculateCoordinates(drawTextInfo);
	::DrawText(drawTextInfo.text, static_cast<int>(position.x), static_cast<int>(position.y), drawTextInfo.fontSize, drawTextInfo.color);
}

void GUI::startFadingInOut(std::function<void()> onFadingInDone, std::function<void()> onFadingOutDone, float seconds)
{
	seconds *= 0.5f;
    m_fading.start(Fade(BLACK, 0.f), Fade(BLACK, 1.f), seconds)
		.onComplete([this, fadingInDone = std::move(onFadingInDone), fadingOutDone = std::move(onFadingOutDone), seconds] {
			fadingInDone();
			m_fading.start(Fade(BLACK, 1.f), Fade(BLACK, 0.f), seconds)
				.onComplete([this, fadingOutDone = std::move(fadingOutDone)] {
					fadingOutDone();
				});
	});
}

Vector2 GUI::calculateCoordinates(const DrawButtonInfo& drawButtonInfo) const
{
	return calculateCoordinates(drawButtonInfo.size, drawButtonInfo.guiPosition);
}

Vector2 GUI::calculateCoordinates(const DrawTextInfo& drawTextInfo) const
{
	auto width = MeasureText(drawTextInfo.text, drawTextInfo.fontSize);
	auto height = drawTextInfo.fontSize;
	return calculateCoordinates({ (float)width, (float)height }, drawTextInfo.guiPosition);
}

Vector2 GUI::calculateCoordinates(const Vector2& size, const GUIPosition& guiPosition) const
{
	auto result = guiPosition.position;

	if (guiPosition.horizontalAlignment.has_value())
	{
		switch (*guiPosition.horizontalAlignment)
		{
		case GUIAlignmentH::Left:
		{
			result.x = guiPosition.position.x;
			break;
		}
		case GUIAlignmentH::Right:
		{
			result.x = TEX_WIDTH - (size.x + guiPosition.position.x);
			break;
		}
		case GUIAlignmentH::Center:
		{
			result.x = TEX_WIDTH / 2 - (size.x / 2) + guiPosition.position.x;
			break;
		}
		}
	}

	if (guiPosition.verticalAlignment.has_value())
	{
		switch (*guiPosition.verticalAlignment)
		{
		case GUIAlignmentV::Top:
		{
			result.y = guiPosition.position.y;
			break;
		}
		case GUIAlignmentV::Bottom:
		{
			result.y = TEX_HEIGHT - (size.y + guiPosition.position.y);
			break;
		}
		case GUIAlignmentV::Center:
		{
			result.y = TEX_HEIGHT / 2 - (size.y / 2) + guiPosition.position.y;
			break;
		}
		}
	}

	return result;
}

void GUI::destroyScreen(const char* name)
{
	auto it = m_screens.find(name);
	if (it != m_screens.end())
	{
		if (m_drawingScreens) 
		{
			m_screensToDestroy.push_back(name);
		}
        else 
		{
            m_screens.erase(it);
        }
    }
}

ScreenBuilder GUI::buildScreen(const char *name) {
	auto screen = std::make_unique<Screen>(name);
	auto screenPtr = screen.get();
	m_screens.insert({ name, std::move(screen) });
	return ScreenBuilder(*screenPtr);
}
