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
		.shape({.color = Fade({20, 20, 20, 255}, 1.f), .type = ShapeType::Rectangle})
		.image({ .pos = {-100, 0}, .sprite = m_game.getGUI().getAtlas().getSpriteInfo("end_screen_00"), .color = Fade(WHITE, 0.4f), .hAlign = HAlign::Center, .vAlign = VAlign::Center, .fit = Fit::Ignore})
		.image({ .pos = {100, 0}, .sprite = m_game.getGUI().getAtlas().getSpriteInfo("end_screen_01"), .color = Fade(WHITE, 0.4f), .hAlign = HAlign::Center, .vAlign = VAlign::Center, .fit = Fit::Ignore})
		.stack({ .orientation = GUIOrientation::Vertical, .padding = {0, 10}, .hAlign = HAlign::Center, .vAlign = VAlign::Center, .sideAlignContent = ContentAlign::Center})
			.medium_text({.text = "Thanks for playing", .color = WHITE})
			.image({ .sprite = gui.getAtlas().getSpriteInfo("logo"), .hAlign = HAlign::Left})
			.stack({ .orientation = GUIOrientation::Horizontal, .padding = {0, 0}, .alignContent = ContentAlign::Center, .sideAlignContent = ContentAlign::Center})
				.image({ .sprite = Animation::createAnimation({m_game.getGUI().getAtlas().getSpriteInfo("shooter_idle"), 0.1f}), .fit = Fit::Ignore})
				.image({ .sprite = Animation::createAnimation({m_game.getGUI().getAtlas().getSpriteInfo("catapult_idle"), 0.1f}), .fit = Fit::Ignore})
				.image({ .sprite = Animation::createAnimation({m_game.getGUI().getAtlas().getSpriteInfo("lasertron_idle"), 0.1f}), .fit = Fit::Ignore})
				.space({10, 0.f})
				.image({ .sprite = Animation::createAnimation({m_game.getGUI().getAtlas().getSpriteInfo("b1_alien_walk"), 0.1f}), .fit = Fit::Ignore})
				.image({ .sprite = Animation::createAnimation({m_game.getGUI().getAtlas().getSpriteInfo("b2_alien_walk"), 0.1f}), .fit = Fit::Ignore})
				.image({ .sprite = Animation::createAnimation({m_game.getGUI().getAtlas().getSpriteInfo("portal_alien_walk"), 0.1f}), .fit = Fit::Ignore})
			.end()
			.space({0, 20.f})
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
