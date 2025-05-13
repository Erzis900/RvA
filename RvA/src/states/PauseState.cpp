#include "states/PauseState.h"

#include "GUI/GUI.h"
#include "Game.h"
#include "MusicManager.h"
#include "Session.h"

PauseState::PauseState(Game& game) : m_game(game) {}

flow::FsmAction PauseState::enter() {
	m_game.getGameSession().setState(SessionState::Paused);

	// clang-format off
	auto btnSize = Vector2{100.f, 30.f};
	auto& gui = m_game.getGUI();	
	gui.buildScreen("Pause")
		.default_bkg(0.7f)
		.stack({ .orientation = GUIOrientation::Vertical, .padding = { 0, 5 }, .size = { 250.f, autoSize }, .sideAlignContent = ContentAlign::Start } )
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 1), Fade(BLACK, 0.0)), .size = {autoSize, 0}, .padding = {5, 0} })
				.big_text({ .text = "PAUSE", .color = WHITE, .hAlign = HAlign::Left, .pos = {10, 0} })
			.end()

			.space({0, 35.f})
		
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({"Resume", {}, btnSize, [this]() { m_nextTransition = "resume"; }})
			.end()
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({"Restart", {}, btnSize, [this]() { restart(); }})
			.end()
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({"Options", {}, btnSize, [this]() { m_nextTransition = "options"; }})
			.end()
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({"Exit to Menu", {}, btnSize, [this]() { exitGameSession(); }})
			.end()
		.end();
	// clang-format on

	return flow::FsmAction::none();
}

flow::FsmAction PauseState::update(float dt) {
	if (!m_nextTransition.empty()) {
		return flow::FsmAction::transition(std::exchange(m_nextTransition, ""));
	}

	if (IsKeyPressed(KEY_ESCAPE)) {
		m_nextTransition = "resume";
	}

	return flow::FsmAction::none();
}

void PauseState::exit() {
	m_game.getGUI().destroyScreen("Pause");
}

void PauseState::exitGameSession() {
	m_game.getGUI().startFadingInOut(
		[this] {
			m_game.getGameSession().setState(SessionState::Idle);
			m_nextTransition = "menu";
		},
		[this] {},
		0.5f);
}

void PauseState::restart() {
	m_game.getGUI().startFadingInOut(
		[this] {
			m_game.getGameSession().setState(SessionState::Idle);
			m_game.getMusicManager().stop(m_game.getMusicManager().getGameMusic());
			m_nextTransition = "restart";
		},
		[this] {},
		0.5f);
}
