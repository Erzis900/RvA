#include "MenuState.h"
#include "Game.h"

MenuState::MenuState(Game& game) : m_game(game)
{
}

flow::FsmAction MenuState::enter()
{
	m_game.getMusicManager().play(m_game.getMusicManager().getMenuMusic());

	auto btnSize = Vector2{ autoSize, 40.f };
	auto& gui = m_game.getGUI();
	gui.buildScreen("MainMenu")
		.vertical_stack(5, 200.f)
		    .medium_text({ .text = "TEMP NAME", .color = WHITE, .hAlign = HAlign::Center })
			.space({ 0, 35.f })
			.button({ "Play", {}, btnSize, [this]() { m_nextTransition = "play"; }})
			.button({ "Options", {}, btnSize, [this]() { m_nextTransition = "options"; }})
			.button({ "Credits", {}, btnSize, [this]() { m_nextTransition = "credits"; }})
			.button({ "Exit", {}, btnSize, [this]() { m_nextTransition = "exit"; }})
		.end();

    return flow::FsmAction::none();
}

flow::FsmAction MenuState::update(float dt)
{
	if (!m_nextTransition.empty()) {
		return flow::FsmAction::transition(std::exchange(m_nextTransition, ""));
	}

    return flow::FsmAction::none();
}

void MenuState::exit()
{
	m_game.getGUI().destroyScreen("MainMenu");
}
