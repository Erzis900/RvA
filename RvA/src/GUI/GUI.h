#pragma once

#include "atlas/Atlas.h"
#include "GUI/Widgets.h"
#include "utilities/Interpolation.h"

#include <raylib.h>
#include <optional>
#include <unordered_map>

class Game;

enum CursorType
{
	Point,
	Hover
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

class MusicManager;

class GUI
{
public:
	GUI(Atlas& atlas, MusicManager& musicManager);

	void loadResources();

	void update(float dt);
	void draw();
	
	void setCursor(CursorType type);

	bool drawButton(DrawButtonInfo drawButtonInfo);
	void drawText(DrawTextInfo drawTextInfo);

	void startFadingInOut(std::function<void()> onFadingInDone, std::function<void()> onFadingOutDone, float seconds);

    void destroyScreen(const char *name);
	ScreenBuilder buildScreen(const char *name);

	void setDefaultButtonSound(Sound* sound);

private:
	Vector2 calculateCoordinates(const DrawButtonInfo& drawButtonInfo) const;
	Vector2 calculateCoordinates(const DrawTextInfo& drawTextInfo) const;
	Vector2 calculateCoordinates(const Vector2& size, const GUIPosition& guiPosition) const;

	void drawCursor();
	void drawFPS();
	void drawScreens();
    void drawWidget(UINode &node, Screen &screen);
	void drawFading();

	Atlas& m_atlas;
    MusicManager& m_musicManager;

	const SpriteInfo* m_mousePointSprite{};
	const SpriteInfo* m_mouseHoverSprite{};
	const SpriteInfo* m_mouseCurrentSprite{};

    Interpolation<Color> m_fading;

	bool m_drawingScreens{};
    std::unordered_map<std::string, std::unique_ptr<Screen>> m_screens;
    std::vector<std::string> m_screensToDestroy;
    Sound* m_defaultButtonSound{};
};