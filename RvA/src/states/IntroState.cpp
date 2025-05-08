#include "IntroState.h"

#include "Game.h"

IntroState::IntroState(Game& game) : m_game(game) {}

flow::FsmAction IntroState::enter() {
	auto& gui = m_game.getGUI();

	// TODO add fade transition and studio logo

	// clang-format off
	gui.buildScreen("Intro")
		.vertical_stack(2, 200.f)
		.medium_text({.text = "Studio name presents:", .color = WHITE, .hAlign = HAlign::Center})
		.small_text({.text = "Game name!", .color = WHITE, .hAlign = HAlign::Center})
		.end();
	// clang-format on

	return flow::FsmAction::none();
}

flow::FsmAction IntroState::update(float dt) {
	m_displayTime -= dt;

	if (m_displayTime <= 0.f) {
		m_game.setRenderTextureColor(GRAY);

		m_nextTransition = "menu";
		return flow::FsmAction::transition(std::exchange(m_nextTransition, ""));
	}

	return flow::FsmAction::none();
}

void IntroState::exit() {
	m_game.getGUI().destroyScreen("Intro");
}
