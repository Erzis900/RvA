#include "GUI.h"
#include <raygui.h>
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

void GUI::drawCursor()
{
	m_atlas.drawSprite(m_mouseCurrentSprite, { GetMousePosition().x - 10, GetMousePosition().y - 5 });
}

void GUI::drawFPS()
{
	auto fpsText = std::to_string(GetFPS());
	drawText({ fpsText.c_str(), 10, GREEN, {{10, 10}, GUIAlignmentH::Right, GUIAlignmentV::Bottom}});
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
