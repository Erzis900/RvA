#pragma once
#include <raylib.h>
#include <optional>

class Game;

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

class GUIHelper
{
public:
	GUIHelper(Game& game);

	bool DrawButton(DrawButtonInfo drawButtonInfo);
	void DrawText(DrawTextInfo drawTextInfo);

private:
	Vector2 calculateCoordinates(const DrawButtonInfo& drawButtonInfo) const;
	Vector2 calculateCoordinates(const DrawTextInfo& drawTextInfo) const;
	Vector2 calculateCoordinates(const Vector2& size, const GUIPosition& guiPosition) const;

	Game& m_game;
};
