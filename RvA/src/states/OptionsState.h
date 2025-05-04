#pragma once

#include "fsm/FsmState.h"
#include "IGameState.h"

#include "GUI/Widgets.h"

class OptionsState : public IGameState, public flow::FsmState
{
public:
	OptionsState(Game& game);

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
	Game& m_game;
};
