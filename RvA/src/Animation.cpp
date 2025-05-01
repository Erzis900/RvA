#include "Animation.h"
#include <iostream>

Animation Animation::createAnimation(AnimationData animationData)
{
	if (animationData.spriteInfo)
	{
		return Animation(animationData.spriteInfo, animationData.frameTime, animationData.loop);
	}
	else
	{
		std::cout << "Error: sprite not found!" << std::endl;
		return Animation(nullptr, 0, 0);
	}
}

Animation::Animation() : Animation(0, 0)
{
}

Animation::Animation(const SpriteInfo* spriteInfo, float frameTime, int loop)
	: m_spriteInfo(spriteInfo), m_frameTime(frameTime), m_loop(loop)
{
}

void Animation::update(float dt)
{
	if (m_loop == 0)
	{
		return;
	}

    m_elapsedTime += dt;

    if (m_elapsedTime >= m_frameTime)
    {
        m_elapsedTime = 0.f;
        m_currentFrame = (m_currentFrame + 1) % m_spriteInfo->num_frames;
    }

	if (m_currentFrame == m_spriteInfo->num_frames - 1)
	{
		--m_loop;
	}
}

void Animation::setLoop(int loop)
{
	m_loop = loop;
}

bool Animation::isOver() const
{
	return m_loop == 0;
}

const SpriteInfo* Animation::getSpriteInfo() const
{
	return m_spriteInfo;
}
