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
	void updateLabels();

	void toggleMusic();
	void toggleSound();
	void toggleFullscreen();
	void toggleTutorial();

	WidgetHandle m_musicButton{};
	WidgetHandle m_windowButton{};
	WidgetHandle m_soundButton{};
	WidgetHandle m_toggleTutorialButton{};
	Screen* m_screen{};
	std::string m_nextTransition{};
	float m_alphaBackground{};
	bool m_playMenuMusic{};
	bool m_showCredits{};
	bool m_playSession{};
	Game& m_game;
};
