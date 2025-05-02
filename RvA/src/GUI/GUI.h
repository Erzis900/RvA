#pragma once

#include "atlas/Atlas.h"
#include <raylib.h>
#include <optional>

class Game;

enum CursorType
{
	Point,
	Hover
};

enum class GUIAlignmentV
{
	Top,
	Bottom,
	Center
};

enum class GUIAlignmentH
{
	Left,
	Right,
	Center
};

struct GUIPosition
{
	// position can be treated as margins when horizontal or vertical alignments are defined
	Vector2 position;
	std::optional<GUIAlignmentH> horizontalAlignment;
	std::optional<GUIAlignmentV> verticalAlignment;
};

struct DrawTextInfo
{
	const char* text{};
	unsigned int fontSize{};
	Color color;
	GUIPosition guiPosition;
};

struct DrawButtonInfo
{
	const char* text{};
	Vector2 size;
	GUIPosition guiPosition;
};

class GUI
{
public:
	GUI(Atlas& atlas);

	void loadResources();

	void drawCursor();
	void drawFPS();

	void setCursor(CursorType type);

	bool drawButton(DrawButtonInfo drawButtonInfo);
	void drawText(DrawTextInfo drawTextInfo);

private:
	Vector2 calculateCoordinates(const DrawButtonInfo& drawButtonInfo) const;
	Vector2 calculateCoordinates(const DrawTextInfo& drawTextInfo) const;
	Vector2 calculateCoordinates(const Vector2& size, const GUIPosition& guiPosition) const;

	Atlas& m_atlas;

	const SpriteInfo* m_mousePointSprite{};
	const SpriteInfo* m_mouseHoverSprite{};
	const SpriteInfo* m_mouseCurrentSprite{};
};