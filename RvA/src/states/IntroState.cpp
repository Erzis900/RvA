#include "IntroState.h"

#include "GUI/GUI.h"
#include "Game.h"
#include "constants.h"

IntroState::IntroState(Game& game) : m_game(game) {}

flow::FsmAction IntroState::enter() {
	if (DEV_MODE) {
		return flow::FsmAction::transition("menu");
	}

	auto& gui = m_game.getGUI();

	// TODO add fade transition and studio logo

	// clang-format off
	gui.buildScreen("Intro")
		.default_bkg()
		.stack({ .orientation = GUIOrientation::Vertical, .padding = {0, 0}, .hAlign = HAlign::Center, .vAlign = VAlign::Center, .sideAlignContent = ContentAlign::Center})
			.medium_text({.text = "Robots vs Aliens!", .color = WHITE})
		.end();
	// clang-format on

	m_exitTime.start(2.f, [this] { m_game.getGUI().startFadingInOut([this] { m_nextTransition = "menu"; }, [this] {}, 0.5f); });

	return flow::FsmAction::none();
}

flow::FsmAction IntroState::update(float dt) {
	m_exitTime.update(dt);

	if (m_nextTransition.empty()) {
		return flow::FsmAction::none();
	} else {
		return flow::FsmAction::transition(m_nextTransition);
	}
}

void IntroState::exit() {
	m_game.getGUI().destroyScreen("Intro");
}
