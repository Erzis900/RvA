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

	m_exitTime.start(2.f, [this] { m_game.getGUI().startFadingInOut([this] { m_nextTransition = "menu"; }, [this] {}, 0.5f); });

	return flow::FsmAction::none();
}

flow::FsmAction IntroState::update(float dt) {
	m_exitTime.update(dt);

	if (m_nextTransition.empty()) {
		return flow::FsmAction::none();
	} else {
		m_game.setRenderTextureColor(GRAY);
		return flow::FsmAction::transition(m_nextTransition);
	}
}

void IntroState::exit() {
	m_game.getGUI().destroyScreen("Intro");
}
