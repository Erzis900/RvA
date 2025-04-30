#include "Animation.h"
#include <iostream>

Animation Animation::createAnimation(const char* name, float frameTime, Atlas& atlas)
{
	texture_atlas_texture_t* textureInfo = texture_atlas_lookup(atlas.getTextureAtlas(), name);
	if (textureInfo)
	{
		return Animation(frameTime, textureInfo->num_frames);
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

Animation::Animation(float frameTime, int totalFrames)
	: m_frameTime(frameTime), m_totalFrames(totalFrames)
{
}

void Animation::update(float dt)
{
    m_elapsedTime += dt;

    if (m_elapsedTime >= m_frameTime)
    {
        m_elapsedTime = 0.f;
        m_currentFrame = (m_currentFrame + 1) % m_totalFrames;
    }
}
