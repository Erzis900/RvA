#pragma once

#include "GUI/Widgets.h"
#include "fsm/FsmState.h"

class Game;

class OptionsState : public flow::FsmState {
public:
	OptionsState(Game& game, bool showBackground, bool playMenuMusic);

	flow::FsmAction update(float dt) override;
	flow::FsmAction enter() override;
	void exit() override;

private:
	void toggleMusic();
	void toggleFullscreen();

	WidgetHandle m_musicButton{};
	WidgetHandle m_windowButton{};
	Screen* m_screen{};
	std::string m_nextTransition{};
	bool m_showBackground{};
	bool m_playMenuMusic{};
	Game& m_game;
};
