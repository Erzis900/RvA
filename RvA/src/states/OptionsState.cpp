#include "OptionsState.h"
#include "Game.h"

OptionsState::OptionsState(Game& game) : m_game(game)
{
}

flow::FsmAction OptionsState::enter()
{
	m_game.getMusicManager().play(m_game.getMusicManager().getMenuMusic());
	
	auto btnSize = Vector2{ autoSize, 40.f };
	auto& gui = m_game.getGUI();
	m_screen = gui.buildScreen("Options")
        .vertical_stack(5, 200.f)
		    .text({ .text = "Options", .fontSize = 20, .color = WHITE, .horizontalAlignment = GUIAlignmentH::Center })
			.space({ 0, 40.f })
			.button({ "Turn off music", {}, btnSize, [this]() { toggleMusic(); }}, &m_musicButton)
			.button({ "Window Mode", {}, btnSize, [this]() { ToggleFullscreen(); } }, &m_windowButton)
			.button({ "Back", {}, btnSize, [this]() { m_nextTransition = "back"; }})
		.end()
	.screen();

    return flow::FsmAction::none();
}

flow::FsmAction OptionsState::update(float dt) {
    if (!m_nextTransition.empty()) {
        return flow::FsmAction::transition(std::exchange(m_nextTransition, ""));
    }

    return flow::FsmAction::none();
}

void OptionsState::exit()
{
	m_game.getMusicManager().stop(m_game.getMusicManager().getMenuMusic());
    m_game.getGUI().destroyScreen("Options");
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
