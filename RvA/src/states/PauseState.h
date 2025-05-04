#pragma once

#include <fsm/FsmState.h>
#include "IGameState.h"

class Game;

class PauseState : public IGameState, public flow::FsmState
{
public:
    PauseState(Game& game);

    flow::FsmAction update(float dt) override;
    flow::FsmAction enter() override;
    void exit() override;

private:
    void exitGameSession();
    void restart();

    std::string m_nextTransition;
    Game& m_game;
};
