#include "OptionsState.h"
#include "Game.h"
#include "states/MenuState.h"

void OptionsState::onEnter(Game& game)
{
	game.getMusicManager().play(game.getMusicManager().getMenuMusic());
	
	auto btnSize = Vector2{ autoSize, 40.f };
	auto& gui = game.getGUI();
	m_screen = gui.buildScreen("Options")
		.stack({ .orientation = GUIOrientation::Vertical, .horizontalAlignment = GUIAlignmentH::Center, .verticalAlignment = GUIAlignmentV::Center, .size = { 100.f, autoSize } })
		    .text({ .text = "Options", .fontSize = 20, .color = WHITE, .horizontalAlignment = GUIAlignmentH::Center })
			.space({ 0, 40.f })
			.button({ "Turn off music", {}, btnSize, [this]() { toggleMusic(); }}, &m_musicButton)
			.button({ "Window Mode", {}, btnSize, [this]() { ToggleFullscreen(); } }, &m_windowButton)
			.button({ "Back", {}, btnSize, [&game]() { game.setState<MenuState>(); } })
		.end()
		.screen();
}

void OptionsState::onExit(Game& game)
{
	game.getMusicManager().stop(game.getMusicManager().getMenuMusic());
    game.getGUI().destroyScreen("Options");
}

void OptionsState::toggleMusic()
{
    if (GetMasterVolume() > 0.f) {
        SetMasterVolume(0.f);
        m_screen->getButton(m_musicButton).text = "Turn on music";
    } else {
        SetMasterVolume(1.0f);
        m_screen->getButton(m_musicButton).text = "Turn off music";
    }
}

void OptionsState::toggleFullscreen() 
{
    if (IsWindowFullscreen()) {
        ToggleFullscreen();
        m_screen->getButton(m_windowButton).text = "Window Mode";
    } else {
        ToggleFullscreen();
        m_screen->getButton(m_windowButton).text = "Fullscreen Mode";
    }
}

void OptionsState::draw(Game& game)
{
}
