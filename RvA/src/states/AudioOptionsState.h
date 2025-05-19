#pragma once

#include "GUI/Widgets.h"
#include "fsm/FsmState.h"

class Game;

class AudioOptionsState : public flow::FsmState {
public:
	AudioOptionsState(Game& game, float alphaBackground, bool playMenuMusic, bool showCredits, bool playSession);

	flow::FsmAction update(float dt) override;
	flow::FsmAction enter() override;
	void exit() override;

private:
	void updateLabels();

	void toggleMusic();
	void toggleSound();

	WidgetHandle m_musicButton{};
	WidgetHandle m_soundButton{};
	WidgetHandle m_musicSliderHandle{};
	WidgetHandle m_musicTextVolumeHandle{};
	WidgetHandle m_soundSliderHandle{};
	WidgetHandle m_soundTextVolumeHandle{};
	WidgetHandle m_masterSliderHandle{};
	WidgetHandle m_masterTextVolumeHandle{};
	Screen* m_screen{};
	std::string m_nextTransition{};
	float m_alphaBackground{};
	bool m_playMenuMusic{};
	bool m_showCredits{};
	bool m_playSession{};
	Game& m_game;
};
