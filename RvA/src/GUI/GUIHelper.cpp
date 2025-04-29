#pragma once
#include "constants.h"
#include "Game.h"
#include "GUIHelper.h"
#include <raygui.h>

GUIHelper::GUIHelper(Game& game) : m_game(game)
{
}

bool GUIHelper::DrawButton(DrawButtonInfo drawButtonInfo)
{
	auto position = calculateCoordinates(drawButtonInfo);
	return ::GuiButton({
		.x = position.x,
		.y = position.y,
		.width = drawButtonInfo.size.x,
		.height = drawButtonInfo.size.y
		}, drawButtonInfo.text);
}

void GUIHelper::DrawText(DrawTextInfo drawTextInfo)
{
	auto position = calculateCoordinates(drawTextInfo);
	::DrawText(drawTextInfo.text, static_cast<int>(position.x), static_cast<int>(position.y), drawTextInfo.fontSize, drawTextInfo.color);
}

Vector2 GUIHelper::calculateCoordinates(const DrawButtonInfo& drawButtonInfo) const
{
	return calculateCoordinates(drawButtonInfo.size, drawButtonInfo.guiPosition);
}

Vector2 GUIHelper::calculateCoordinates(const DrawTextInfo& drawTextInfo) const
{
	auto width = MeasureText(drawTextInfo.text, drawTextInfo.fontSize);
	auto height = drawTextInfo.fontSize;
	return calculateCoordinates({ (float)width, (float)height }, drawTextInfo.guiPosition);
}

Vector2 GUIHelper::calculateCoordinates(const Vector2& size, const GUIPosition& guiPosition) const
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
