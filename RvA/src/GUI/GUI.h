#pragma once

#include "GUI/Widgets.h"
#include "atlas/Atlas.h"
#include "utilities/Interpolation.h"

#include <optional>
#include <raylib.h>
#include <unordered_map>

class Game;

enum CursorType {
	Point,
	Hover
};

class MusicManager;

class GUI {
public:
	GUI(Atlas& atlas, MusicManager& musicManager);
	~GUI();

	void loadResources();

	void update(float dt);
	void draw();

	void setCursor(CursorType type);

	void startFadingInOut(std::function<void()> onFadingInDone, std::function<void()> onFadingOutDone, float seconds);

	void destroyScreen(const char* name);
	ScreenBuilder buildScreen(const char* name);

	void setDefaultButtonSound(Sound* sound);

	void toggleDebugView();

	auto& getAtlas() {
		return m_atlas;
	}

private:
	void drawCursor();
	void drawFPS();
	void drawScreens();
	void drawWidget(UINode& node, Screen& screen);
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
	bool m_isDebugViewEnabled{false};
	Font m_font{};
};
