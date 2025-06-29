#include "LostState.h"

#include "GUI/GUI.h"
#include "Game.h"
#include "MusicManager.h"
#include "Session.h"

LostState::LostState(Game& game) : m_game(game) {}

flow::FsmAction LostState::enter() {
	m_game.getMusicManager().playMusic("lost");

	// clang-format off
	auto btnSize = Vector2{100, 30.f};
	auto& gui = m_game.getGUI();
	gui.buildScreen("Lose")
		.default_bkg(0.5f)
		.stack({ .orientation = GUIOrientation::Vertical, .padding = {0, 10}, .hAlign = HAlign::Center, .vAlign = VAlign::Center, .sideAlignContent = ContentAlign::Center})
			.medium_text({.text = "You Lost!", .color = WHITE, .hAlign = HAlign::Center})
			.space({0, 35.f})
			.button({"Restart", {}, btnSize, [this]() { restart(); }})
			.button({"Menu", {}, btnSize, [this]() { goToMenu(); }})
		.end();
	// clang-format on

	return flow::FsmAction::none();
}

flow::FsmAction LostState::update(float dt) {
	if (!m_nextTransition.empty()) {
		return flow::FsmAction::transition(std::exchange(m_nextTransition, ""));
	}

	return flow::FsmAction::none();
}

void LostState::exit() {
	m_game.getGUI().destroyScreen("Lose");
}

void LostState::restart() {
	m_game.getGUI().startFadingInOut([this] { m_nextTransition = "restart"; }, [this] {}, 0.5f);
}

void LostState::goToMenu() {
	m_game.getGUI().startFadingInOut([this] { m_nextTransition = "menu"; }, [this] {}, 0.5f);
}
