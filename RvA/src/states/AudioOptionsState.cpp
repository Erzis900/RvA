#include "AudioOptionsState.h"

#include "Config.h"
#include "CreditsHelper.h"
#include "GUI/GUI.h"
#include "Game.h"
#include "MusicManager.h"
#include "Session.h"
#include "constants.h"

AudioOptionsState::AudioOptionsState(Game& game, float alphaBackground, bool playMenuMusic, bool showCredits, bool playSession)
	: m_game(game)
	, m_alphaBackground(alphaBackground)
	, m_playMenuMusic(playMenuMusic)
	, m_showCredits(showCredits)
	, m_playSession(playSession) {}

flow::FsmAction AudioOptionsState::enter() {
	if (m_playMenuMusic) {
		m_game.getMusicManager().playMusic("menu");
	}

	auto btnSize = Vector2{100.f, 30.f};
	auto sliderSize = Vector2{100.f, 10.f};
	auto& gui = m_game.getGUI();
	auto& config = m_game.getConfig();

	auto builder = gui.buildScreen("Options");

	// clang-format off
	builder
		.default_bkg(m_alphaBackground)
		.stack({ .orientation = GUIOrientation::Vertical, .padding = { 0, 5 }, .size = Vec2{ 250.f, autoSize }, .sideAlignContent = ContentAlign::Start } )
			.image({.pos = {10, 10}, .sprite = gui.getAtlas().getSpriteInfo("logo"), .hAlign = HAlign::Left})

			.space({0, 35.f})
		
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .size = {autoSize, 30.f}, .padding = {5, 0} })
				.stack({ .orientation = GUIOrientation::Horizontal, .padding = {10, 0}, .vAlign = VAlign::Center, .sideAlignContent = ContentAlign::Center })
					.small_text({ .text = "Master Volume", .color = WHITE, .hAlign = HAlign::Left })
					.slider({ .min = 0.f, .max = 1.f, .value = m_game.getMusicManager().getMasterVolume(), .step = 0.1f, .pos = {26, 0}, .size = sliderSize}, &m_masterSliderHandle)
					.small_text({ .text = "", .color = WHITE, .hAlign = HAlign::Left, .pos = {36, 0} }, &m_masterTextVolumeHandle)
				.end()
			.end()
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.stack({ .orientation = GUIOrientation::Horizontal, .padding = {10, 0}, .sideAlignContent = ContentAlign::Center })
					.label_button({"", {}, btnSize, [this]() { toggleMusic(); }}, &m_musicButton)
					.slider({ .min = 0.f, .max = 1.f, .value = m_game.getMusicManager().getMusicVolume(), .step = 0.1f, .size = sliderSize}, &m_musicSliderHandle)
					.small_text({ .text = "", .color = WHITE, .hAlign = HAlign::Left, .pos = {10, 0} }, &m_musicTextVolumeHandle)
				.end()
			.end()
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.stack({ .orientation = GUIOrientation::Horizontal, .padding = {10, 0}, .sideAlignContent = ContentAlign::Center })
					.label_button({"", {}, btnSize, [this]() { toggleSound(); }}, &m_soundButton)
					.slider({ .min = 0.f, .max = 1.f, .value = m_game.getMusicManager().getSoundVolume(), .step = 0.1f, .size = sliderSize}, &m_soundSliderHandle)
					.small_text({ .text = "", .color = WHITE, .hAlign = HAlign::Left, .pos = {10, 0} }, &m_soundTextVolumeHandle)
				.end()
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

flow::FsmAction AudioOptionsState::update(float dt) {
	if (m_playSession) {
		m_game.getGameSession().update(dt);
	}

	if (!m_nextTransition.empty()) {
		return flow::FsmAction::transition(std::exchange(m_nextTransition, ""));
	}

	auto& musicSlider = m_screen->getSlider(m_musicSliderHandle);
	auto& musicText = m_screen->getText(m_musicTextVolumeHandle);
	auto& soundSlider = m_screen->getSlider(m_soundSliderHandle);
	auto& soundText = m_screen->getText(m_soundTextVolumeHandle);
	auto& masterSlider = m_screen->getSlider(m_masterSliderHandle);
	auto& masterText = m_screen->getText(m_masterTextVolumeHandle);

	musicText.text = TextFormat("%d", static_cast<int>(musicSlider.value * 100));
	soundText.text = TextFormat("%d", static_cast<int>(soundSlider.value * 100));
	masterText.text = TextFormat("%d", static_cast<int>(masterSlider.value * 100));

	if (masterSlider.value != m_game.getMusicManager().getMasterVolume()) {
		m_game.getMusicManager().setMasterVolume(masterSlider.value);
	}

	if (musicSlider.value != m_game.getMusicManager().getMusicVolume()) {
		m_game.getMusicManager().setMusicVolume(musicSlider.value);
	}

	if (soundSlider.value != m_game.getMusicManager().getSoundVolume()) {
		m_game.getMusicManager().setSoundVolume(soundSlider.value);
	}

	return flow::FsmAction::none();
}

void AudioOptionsState::exit() {
	m_game.getGUI().destroyScreen("Options");
}

void AudioOptionsState::updateLabels() {
	m_screen->getButton(m_musicButton).text = m_game.getMusicManager().isMusicEnabled() ? "Music Volume ON" : "Music Volume OFF";
	m_screen->getButton(m_soundButton).text = m_game.getMusicManager().isSoundEnabled() ? "Sound Volume ON" : "Sound Volume OFF";
}

// TODO SetMasterVolume includes volume of ALL sounds, MusicManager should handle Volume to differentiate Music and Sound Volume
// TODO add Volume slider in options screen
void AudioOptionsState::toggleMusic() {
	m_game.getMusicManager().toggleMusic();
	updateLabels();
}

void AudioOptionsState::toggleSound() {
	m_game.getMusicManager().toggleSound();
	updateLabels();
}
