#include "PlayState.h"

#include "Game.h"
#include "constants.h"

PlayState::PlayState(Game& game) : m_game(game) {}

flow::FsmAction PlayState::enter() {
	m_game.getGameSession().start();
	m_game.getMusicManager().play(m_game.getMusicManager().getGameMusic());

	return flow::FsmAction::none();
}

flow::FsmAction PlayState::update(float dt) {
	Session& session = m_game.getGameSession();
	session.update(dt);

	switch (session.getGameState()) {
	case GameState::Win : return flow::FsmAction::transition("win");
	case GameState::Lost: return flow::FsmAction::transition("lost");
	}

	if constexpr (DEV_MODE) {
		if (IsKeyPressed(KEY_F1)) {
			session.getCollisionSystem().toggleDebugView();
		}
	}

	if (IsKeyPressed(KEY_ESCAPE)) {
		return flow::FsmAction::transition("pause");
	}

	return flow::FsmAction::none();
}

void PlayState::exit() {}
