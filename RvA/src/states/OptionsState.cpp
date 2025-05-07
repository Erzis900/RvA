#include "OptionsState.h"

#include "Game.h"

OptionsState::OptionsState(Game& game, bool showBackground) : m_game(game), m_showBackground(showBackground) {}

flow::FsmAction OptionsState::enter() {
	m_game.getMusicManager().play(m_game.getMusicManager().getMenuMusic());

	auto btnSize = Vector2{autoSize, 40.f};
	auto& gui = m_game.getGUI();
	// clang-format off
	auto builder = gui.buildScreen("Options");
	if(m_showBackground) {
		builder.rect({0, 0, TEX_WIDTH, TEX_HEIGHT}, Fade(BLACK, 0.5f));
	}

	builder
		.vertical_stack(5, 200.f)
			.medium_text({.text = "Options", .color = WHITE, .hAlign = HAlign::Center})
			.space({0, 40.f})
			.button({"Turn off music", {}, btnSize, [this]() { toggleMusic(); }}, &m_musicButton)
			.button({"Window Mode", {}, btnSize, [this]() { ToggleFullscreen(); }}, &m_windowButton)
			.button({"Back", {}, btnSize, [this]() { m_nextTransition = "back"; }})
		.end();
	// clang-format on

	m_screen = builder.screen();
	return flow::FsmAction::none();
}

flow::FsmAction OptionsState::update(float dt) {
	if (!m_nextTransition.empty()) {
		return flow::FsmAction::transition(std::exchange(m_nextTransition, ""));
	}

	return flow::FsmAction::none();
}

void OptionsState::exit() {
	m_game.getGUI().destroyScreen("Options");
}

// TODO SetMasterVolume includes volume of ALL sounds, MusicManager should handle Volume to differentiate Music and Sound Volume
// TODO add Volume slider in options screen
void OptionsState::toggleMusic() {
	if (GetMasterVolume() > 0.f) {
		SetMasterVolume(0.f);
		m_screen->getButton(m_musicButton).text = "Turn on sound";
	} else {
		SetMasterVolume(1.0f);
		m_screen->getButton(m_musicButton).text = "Turn off sound";
	}
}

void OptionsState::toggleFullscreen() {
	if (IsWindowFullscreen()) {
		ToggleFullscreen();
		m_screen->getButton(m_windowButton).text = "Window Mode";
	} else {
		ToggleFullscreen();
		m_screen->getButton(m_windowButton).text = "Fullscreen Mode";
	}
}
