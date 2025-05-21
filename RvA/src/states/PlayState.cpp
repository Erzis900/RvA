#include "PlayState.h"

#include "GUI/GUI.h"
#include "Game.h"
#include "Session.h"

PlayState::PlayState(Game& game) : m_game(game) {}

flow::FsmAction PlayState::enter() {
	return flow::FsmAction::none();
}

flow::FsmAction PlayState::update(float dt) {
	Session& session = m_game.getGameSession();
	session.update(dt);

	switch (session.getGameState()) {
	case SessionState::Win : return flow::FsmAction::transition("win");
	case SessionState::End : return flow::FsmAction::transition("end");
	case SessionState::Lost: return flow::FsmAction::transition("lost");
	case SessionState::Skip: return flow::FsmAction::transition("skip");
	}

	if (IsKeyPressed(KEY_ESCAPE)) {
		return flow::FsmAction::transition("pause");
	}

	return flow::FsmAction::none();
}

void PlayState::exit() {
	m_game.getGUI().setCursor(CursorType::Default);
}
