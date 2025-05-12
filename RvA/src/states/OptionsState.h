#pragma once

#include "GUI/Widgets.h"
#include "fsm/FsmState.h"

class Game;

class OptionsState : public flow::FsmState {
public:
	OptionsState(Game& game, float alphaBackground, bool playMenuMusic, bool showCredits, bool playSession);

	flow::FsmAction update(float dt) override;
	flow::FsmAction enter() override;
	void exit() override;

private:
	std::string getFullscreenString(bool isFullscreen);
	std::string getMusicString(bool isMusic);
	std::string getSoundString(bool isSound);

	void toggleMusic();
	void toggleSound();
	void toggleFullscreen();

	WidgetHandle m_musicButton{};
	WidgetHandle m_windowButton{};
	WidgetHandle m_soundButton{};
	Screen* m_screen{};
	std::string m_nextTransition{};
	float m_alphaBackground{};
	bool m_playMenuMusic{};
	bool m_showCredits{};
	bool m_playSession{};
	Game& m_game;
};
