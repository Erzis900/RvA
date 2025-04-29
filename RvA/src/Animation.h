#pragma once
#include "atlas/Atlas.h"

class Animation
{
public:
    Animation(std::string name, float frameTime, Atlas& atlas);

    void update(float dt);
    int getCurrentFrame() const { return m_currentFrame; }
private:
    const char* m_name{};
    float m_frameTime{};
    int m_currentFrame{};
    float m_elapsedTime{};
    int m_totalFrames{};
};
