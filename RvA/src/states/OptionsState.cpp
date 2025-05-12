#include "OptionsState.h"

#include "Config.h"
#include "GUI/GUI.h"
#include "Game.h"
#include "MusicManager.h"
#include "constants.h"

OptionsState::OptionsState(Game& game, bool showBackground, bool playMenuMusic) : m_game(game), m_showBackground(showBackground), m_playMenuMusic(playMenuMusic) {}

flow::FsmAction OptionsState::enter() {
	if (m_playMenuMusic) {
		m_game.getMusicManager().play(m_game.getMusicManager().getMenuMusic());
	}

	auto btnSize = Vector2{autoSize, 40.f};
	auto& gui = m_game.getGUI();
	auto& config = m_game.getConfig();

	auto builder = gui.buildScreen("Options");
	if (m_showBackground) {
		builder.rect({0, 0, UI_RENDERTEXTURE_SIZE.x, UI_RENDERTEXTURE_SIZE.y}, Fade(BLACK, 0.5f));
	}

	// clang-format off
	builder.vertical_stack(5, 200.f)
		.medium_text({.text = "Options", .color = WHITE, .hAlign = HAlign::Center})
		.space({0, 40.f})
		.button({getMusicString(config.options.isMusic), {}, btnSize, [this]() { toggleMusic(); }}, &m_musicButton)
		.button({getSoundString(config.options.isSound), {}, btnSize, [this]() { toggleSound(); }}, &m_soundButton)
		.button({getFullscreenString(config.options.isFullscreen), {}, btnSize, [this]() { ToggleFullscreen(); }}, &m_windowButton)
		.button({"Back", {}, btnSize, [this, &config]() { config.save(); m_nextTransition = "back"; }})
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
	bool& isMusic = m_game.getConfig().options.isMusic;
	isMusic = !isMusic;

	// isMusic ? SetMasterVolume(1.f) : SetMasterVolume(0.0f);

	m_screen->getButton(m_musicButton).text = getMusicString(isMusic);
}

void OptionsState::toggleSound() {
	bool& isSound = m_game.getConfig().options.isSound;
	isSound = !isSound;

	m_screen->getButton(m_soundButton).text = getSoundString(isSound);
}

void OptionsState::toggleFullscreen() {
	bool& isFullscreen = m_game.getConfig().options.isFullscreen;
	ToggleFullscreen();

	m_screen->getButton(m_windowButton).text = getFullscreenString(isFullscreen);
}

std::string OptionsState::getFullscreenString(bool isFullscreen) {
	return isFullscreen ? "Full screen" : "Windowed";
}

std::string OptionsState::getMusicString(bool isMusic) {
	return isMusic ? "Music ON" : "Music OFF";
}

std::string OptionsState::getSoundString(bool isSound) {
	return isSound ? "Sound ON" : "Sound OFF";
}
