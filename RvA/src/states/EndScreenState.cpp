#include "EndScreenState.h"

#include "GUI/GUI.h"
#include "Game.h"
#include "MusicManager.h"
#include "Session.h"

EndScreenState::EndScreenState(Game& game) : m_game(game) {}

flow::FsmAction EndScreenState::enter() {
	m_game.getMusicManager().playMusic("menu");
	auto& gui = m_game.getGUI();
	// clang-format off
	gui.buildScreen("EndScreen")
		.default_bkg(0.5f)
		.stack({ .orientation = GUIOrientation::Vertical, .padding = {0, 5}, .hAlign = HAlign::Center, .vAlign = VAlign::Center, .sideAlignContent = ContentAlign::Center})
			.medium_text({.text = "Thank you for playing!", .color = WHITE})
			.space({0, 35.f})
			.button({"Main Menu", {}, {100, 30.f}, [this]() { goToMenu(); }})
		.end();
	// clang-format on
	return flow::FsmAction::none();
}

void EndScreenState::exit() {
	m_game.getGUI().destroyScreen("EndScreen");
}

flow::FsmAction EndScreenState::update(float dt) {
	if (!m_nextTransition.empty()) {
		return flow::FsmAction::transition(std::exchange(m_nextTransition, ""));
	}
	return flow::FsmAction::none();
}

void EndScreenState::goToMenu() {
	m_game.getGUI().startFadingInOut([this] { m_nextTransition = "menu"; }, [this] {}, 0.5f);
}
