#pragma once
#include "atlas/Atlas.h"

class Animation
{
public:
    Animation();
    Animation(float frameTime, int totalFrames);

    void update(float dt);
    int getCurrentFrame() const { return m_currentFrame; }

    static Animation createAnimation(const char* name, float frameTime, Atlas& atlas);
private:
    float m_frameTime{};
    int m_currentFrame{};
    float m_elapsedTime{};
    int m_totalFrames{};
};
