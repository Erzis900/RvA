#include "states/PauseState.h"

#include "Game.h"

PauseState::PauseState(Game& game) : m_game(game) {}

flow::FsmAction PauseState::enter() {
	m_game.getGameSession().setState(SessionState::Paused);

	auto btnSize = Vector2{autoSize, 40.f};
	auto& gui = m_game.getGUI();
	gui.buildScreen("Pause")
		.rect({0, 0, UI_RENDERTEXTURE_SIZE.x, UI_RENDERTEXTURE_SIZE.y}, Fade(BLACK, 0.5f))
		.vertical_stack(5, 200.f)
		.medium_text({.text = "Pause", .color = WHITE, .hAlign = HAlign::Center})
		.space({0, 40.f})
		.button({"Resume", {}, btnSize, [this]() { m_nextTransition = "resume"; }})
		.button({"Restart", {}, btnSize, [this]() { restart(); }})
		.button({"Options", {}, btnSize, [this]() { m_nextTransition = "options"; }})
		.button({"Exit to Menu", {}, btnSize, [this]() { exitGameSession(); }})
		.end();

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
	m_game.getGameSession().setState(SessionState::Idle);
	m_nextTransition = "menu";
}

void PauseState::restart() {
	m_game.getGameSession().setState(SessionState::Idle);
	m_game.getMusicManager().stop(m_game.getMusicManager().getGameMusic());
	m_nextTransition = "restart";
}
