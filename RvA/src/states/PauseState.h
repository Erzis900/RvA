#pragma once

#include "IGameState.h"

class Session;

class PauseState : public IGameState {
public:
    PauseState(Session& gameSession);

    void update(Game& game, float dt) override;
    void draw(Game& game) override; 
    void onEnter(Game& game) override;
    void onExit(Game& game) override;

private:
    void exitGameSession(Game& game);

    Session& m_gameSession;
};
