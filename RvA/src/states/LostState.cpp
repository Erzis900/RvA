#include "LostState.h"

#include "Game.h"

LostState::LostState(Game& game) : m_game(game)
{
}

flow::FsmAction LostState::enter()
{
	m_game.getGameSession().end();
	m_game.getMusicManager().play(m_game.getMusicManager().getLostMusic());

	auto btnSize = Vector2{ autoSize, 40.f };
	auto& gui = m_game.getGUI();
	gui.buildScreen("Lose")
		.vertical_stack(5, 200.f)
		    .medium_text({ .text = "You Lost!", .color = WHITE, .hAlign = HAlign::Center })
			.space({ 0, 35.f })
			.button({ "Restart", {}, btnSize, [this]() { restart(); }})
			.button({ "Menu", {}, btnSize, [this]() { m_nextTransition = "menu"; }})
		.end();

    return flow::FsmAction::none();
}

flow::FsmAction LostState::update(float dt) {
    if (!m_nextTransition.empty()) {
        return flow::FsmAction::transition(std::exchange(m_nextTransition, ""));
    }

    return flow::FsmAction::none();
}

void LostState::exit()
{
	m_game.getGUI().destroyScreen("Lose");
}

void LostState::restart()
{
	m_game.getGameSession().end();
    m_nextTransition = "restart";
}
