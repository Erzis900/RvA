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
		m_game.getMusicManager().playMusic("menu");
	}

	auto btnSize = Vector2{100.f, 30.f};
	auto& gui = m_game.getGUI();
	auto& config = m_game.getConfig();

	auto builder = gui.buildScreen("Options");

	// clang-format off
	builder
		.default_bkg(m_alphaBackground)
		.stack({ .orientation = GUIOrientation::Vertical, .padding = { 0, 5 }, .size = Vec2{ 250.f, autoSize }, .sideAlignContent = ContentAlign::Start } )
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 1), Fade(BLACK, 0.0)), .size = {autoSize, 0}, .padding = {5, 0} })
				.big_text({ .text = "OPTIONS", .color = WHITE, .hAlign = HAlign::Left, .pos = {10, 0} })
			.end()

			.space({0, 35.f})
		
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({"Audio", {}, btnSize, [this]() { m_nextTransition = "audio"; }})
			.end()
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({"", {}, btnSize, [this]() { ToggleFullscreen(); }}, &m_windowButton)
			.end()
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({"", {}, btnSize, [this]() { toggleTutorial(); }}, &m_toggleTutorialButton)
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

	updateLabels();

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

void OptionsState::updateLabels() {
	auto& options = m_game.getConfig().options;
	m_screen->getButton(m_windowButton).text = options.isFullscreen ? "Full screen" : "Windowed";
	m_screen->getButton(m_toggleTutorialButton).text = options.isTutorialEnabled ? "Tutorial ON" : "Tutorial OFF";
}

void OptionsState::toggleFullscreen() {
	auto& options = m_game.getConfig().options;
	options.isFullscreen = !options.isFullscreen;
	ToggleBorderlessWindowed();
	updateLabels();
}

void OptionsState::toggleTutorial() {
	bool& isTutorial = m_game.getConfig().options.isTutorialEnabled;
	isTutorial = !isTutorial;
	updateLabels();
}
