#pragma once
#include "atlas/Atlas.h"

class Animation
{
public:
    Animation();
    Animation(float frameTime, int totalFrames, int loop = -1);

    void update(float dt);
    int getCurrentFrame() const { return m_currentFrame; }
    void setLoop(int loop);
    bool isOver() const;

    static Animation createAnimation(const char* name, float frameTime, Atlas& atlas, int loop = -1);

private:
    float m_frameTime{};
    int m_currentFrame{};
    float m_elapsedTime{};
    int m_totalFrames{};
    int m_loop{ -1 }; // -1 = infinite loop, N = execute the animation N times
};
