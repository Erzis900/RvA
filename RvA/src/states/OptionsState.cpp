#include "OptionsState.h"

#include "Config.h"
#include "CreditsHelper.h"
#include "GUI/GUI.h"
#include "Game.h"
#include "MusicManager.h"
#include "Session.h"
#include "constants.h"

OptionsState::OptionsState(Game& game, float alphaBackground, bool playMenuMusic, bool showCredits, bool playSession)
	: m_game(game)
	, m_alphaBackground(alphaBackground)
	, m_playMenuMusic(playMenuMusic)
	, m_showCredits(showCredits)
	, m_playSession(playSession) {}

flow::FsmAction OptionsState::enter() {
	if (m_playMenuMusic) {
		m_game.getMusicManager().play(m_game.getMusicManager().getMenuMusic());
	}

	auto btnSize = Vector2{100.f, 30.f};
	auto& gui = m_game.getGUI();
	auto& config = m_game.getConfig();

	auto builder = gui.buildScreen("Options");

	// clang-format off
	builder
		.default_bkg(m_alphaBackground)
		.stack({ .orientation = GUIOrientation::Vertical, .padding = { 0, 5 }, .size = { 250.f, autoSize }, .sideAlignContent = ContentAlign::Start } )
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 1), Fade(BLACK, 0.0)), .size = {autoSize, 0}, .padding = {5, 0} })
				.big_text({ .text = "OPTIONS", .color = WHITE, .hAlign = HAlign::Left, .pos = {10, 0} })
			.end()

			.space({0, 35.f})
		
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({getMusicString(config.options.isMusic), {}, btnSize, [this]() { toggleMusic(); }}, &m_musicButton)
			.end()
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({getSoundString(config.options.isSound), {}, btnSize, [this]() { toggleSound(); }}, &m_soundButton)
			.end()
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({getFullscreenString(config.options.isFullscreen), {}, btnSize, [this]() { ToggleFullscreen(); }}, &m_windowButton)
			.end()
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({"Back", {}, btnSize, [this, &config]() { config.save(); m_nextTransition = "back"; }})
			.end()
		.end();
	// clang-format on

	if (m_showCredits) {
		CreditsHelper::fillCredits(builder, gui);
	}

	m_screen = builder.screen();
	return flow::FsmAction::none();
}

flow::FsmAction OptionsState::update(float dt) {
	if (m_playSession) {
		m_game.getGameSession().update(dt);
	}

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
