#include "Animation.h"
#include <iostream>

Animation Animation::createAnimation(const char* name, float frameTime, Atlas& atlas, int loop)
{
	texture_atlas_texture_t* textureInfo = texture_atlas_lookup(atlas.getTextureAtlas(), name);
	if (textureInfo)
	{
		return Animation(frameTime, textureInfo->num_frames, loop);
	}
	else
	{
		std::cout << "Error: sprite " << name << " not found!" << std::endl;
		return Animation(frameTime, 0);
	}
}

Animation::Animation() : Animation(0, 0)
{
}

Animation::Animation(float frameTime, int totalFrames, int loop)
	: m_frameTime(frameTime), m_totalFrames(totalFrames), m_loop(loop)
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
        m_currentFrame = (m_currentFrame + 1) % m_totalFrames;
    }

	if (m_currentFrame == m_totalFrames - 1)
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
