#include "PlayState.h"

#include "GUI/GUI.h"
#include "Game.h"
#include "MusicManager.h"
#include "Session.h"
#include "constants.h"

PlayState::PlayState(Game& game) : m_game(game) {}

flow::FsmAction PlayState::enter() {
	m_game.getGameSession().setState(SessionState::Playing);
	m_game.getMusicManager().play(m_game.getMusicManager().getGameMusic());

	return flow::FsmAction::none();
}

flow::FsmAction PlayState::update(float dt) {
	Session& session = m_game.getGameSession();
	session.update(dt);

	switch (session.getGameState()) {
	case SessionState::Win : return flow::FsmAction::transition("win");
	case SessionState::End : return flow::FsmAction::transition("end");
	case SessionState::Lost: return flow::FsmAction::transition("lost");
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

void PlayState::exit() {
	m_game.getGUI().setCursor(CursorType::Default);
}
