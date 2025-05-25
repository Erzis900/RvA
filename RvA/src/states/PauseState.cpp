#include "states/PauseState.h"

#include "GUI/GUI.h"
#include "Game.h"
#include "MusicManager.h"
#include "Session.h"

PauseState::PauseState(Game& game) : m_game(game) {}

flow::FsmAction PauseState::enter() {
	// clang-format off
	auto btnSize = Vector2{100.f, 30.f};
	auto& gui = m_game.getGUI();	
	gui.buildScreen("Pause")
		.default_bkg(0.7f)
		.stack({ .orientation = GUIOrientation::Vertical, .padding = { 0, 5 }, .size = Vec2{ 250.f, autoSize }, .sideAlignContent = ContentAlign::Start } )
			.image({.pos = {10, 10}, .sprite = gui.getAtlas().getSpriteInfo("logo"), .hAlign = HAlign::Left})

			.space({0, 35.f})
		
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({"Resume", {}, btnSize, [this]() { m_nextTransition = "resume"; }})
			.end()
			.border({ .color = Fade(BLACK, 0.0), .bkgColor = std::make_pair(Fade(BLACK, 0.5), Fade(BLACK, 0.0)), .padding = {5, 0} })
				.label_button({"Restart Level", {}, btnSize, [this]() { restart(); }})
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
	m_game.getGUI().startFadingInOut([this] { m_nextTransition = "menu"; }, [this] {}, 0.5f);
}

void PauseState::restart() {
	m_game.getGUI().startFadingInOut(
		[this] {
			m_game.getMusicManager().stopMusic();
			m_nextTransition = "restart";
		},
		[this] {},
		0.5f);
}
