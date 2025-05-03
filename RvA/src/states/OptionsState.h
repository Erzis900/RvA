#pragma once

#include "IGameState.h"

#include "GUI/Widgets.h"

#include <raylib.h>

class OptionsState : public IGameState
{
public:
	void draw(Game& game) override;
	void onEnter(Game& game) override;
	void onExit(Game& game) override;

private:
    void toggleMusic();
	void toggleFullscreen();

	WidgetHandle m_musicButton{};
	WidgetHandle m_windowButton{};
    Screen* m_screen{};
};
