#include "MenuState.h"

#include "CreditsHelper.h"
#include "GUI/GUI.h"
#include "Game.h"
#include "MusicManager.h"
#include "Session.h"

MenuState::MenuState(Game& game) : m_game(game) {}

flow::FsmAction MenuState::enter() {
	m_game.getMusicManager().playMusic("menu");

	// clang-format off
	auto btnSize = Vector2{100.f, 30.f};
	auto& gui = m_game.getGUI();
	auto screenBuilder = gui.buildScreen("MainMenu");
		
	screenBuilder
		.default_bkg(0.25f)
		.stack({ .orientation = GUIOrientation::Vertical, .padding = { 0, 5 }, .size = Vec2{ 250.f, autoSize }, .sideAlignContent = ContentAlign::Start } )
			.image({.pos = {10, 10}, .sprite = gui.getAtlas().getSpriteInfo("logo"), .hAlign = HAlign::Left})

			.space({0, 35.f})
		
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({ .text = "Play Demo", .size = btnSize, .onClick = [this]() { startGame(); }, .hAlign = HAlign::Left, .vAlign = VAlign::Center })
			.end()
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({ .text = "Options", .size = btnSize, .onClick = [this]() { m_nextTransition = "options"; }, .hAlign = HAlign::Left, .vAlign = VAlign::Center })
			.end()
		#ifndef WEB_MODE
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({ .text = "Exit", .size = btnSize, .onClick = [this]() { m_nextTransition = "exit"; }, .hAlign = HAlign::Left, .vAlign = VAlign::Center })
			.end()
		#endif
		.end();

	CreditsHelper::fillCredits(screenBuilder, gui);
	// clang-format on

	return flow::FsmAction::none();
}

flow::FsmAction MenuState::update(float dt) {
	if (!m_nextTransition.empty()) {
		return flow::FsmAction::transition(std::exchange(m_nextTransition, ""));
	}

	m_game.getGameSession().update(dt);

	return flow::FsmAction::none();
}

void MenuState::exit() {
	m_game.getGUI().destroyScreen("MainMenu");
}

void MenuState::startGame() {
	m_game.getGUI().startFadingInOut([this] { m_nextTransition = "play"; }, [this] {}, 0.5f);
}
