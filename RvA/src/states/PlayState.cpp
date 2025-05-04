#include "PlayState.h"
#include "Game.h"
#include "constants.h"

PlayState::PlayState(Game& game)
	: m_game(game)
{
}

flow::FsmAction PlayState::enter() {
	m_game.getGameSession().start();
	m_game.getMusicManager().play(m_game.getMusicManager().getGameMusic());

    return flow::FsmAction::none();
}

flow::FsmAction PlayState::update(float dt) {
	Session& session = m_game.getGameSession();
	session.update(dt);

	if (session.getNumberOfDestroyedEnemies() >= m_numberOfEnemiesToKill) {
		return flow::FsmAction::transition("win");
	}

	if (session.getBatteryCharge() <= 0) {
		return flow::FsmAction::transition("lost");
	}

	if constexpr (DEV_MODE) {
		if (IsKeyPressed(KEY_W)) {
			return flow::FsmAction::transition("win");
		} else if (IsKeyPressed(KEY_F1)) {
			session.getCollisionSystem().toggleDebugView();
		}
	}

	if (IsKeyPressed(KEY_ESCAPE)) {
		return flow::FsmAction::transition("pause");
	}

	return flow::FsmAction::none();
}

void PlayState::exit() {
    m_game.getMusicManager().stop(m_game.getMusicManager().getGameMusic());
}