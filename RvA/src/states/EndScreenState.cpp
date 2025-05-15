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
		.vertical_stack(5, 200.f)
			.medium_text({.text = "Thank you for playing!", .color = WHITE, .hAlign = HAlign::Center})
			.space({0, 35.f})
			.button({"Main Menu", {}, {autoSize, 40.f}, [this]() { goToMenu(); }})
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
