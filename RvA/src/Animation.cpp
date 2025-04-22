#include "Animation.h"
#include <iostream>

Animation::Animation(std::string& name, float frameTime, Atlas& atlas)
	: m_name(name), m_frameTime(frameTime), m_currentFrame(0), m_elapsedTime(0.f), m_atlas(atlas)
{
	texture_atlas_texture_t* textureInfo = texture_atlas_lookup(m_atlas.getTextureAtlas(), m_name.c_str());
	if (textureInfo)
	{
		m_totalFrames = textureInfo->num_frames;
	}
	else
	{
		std::cout << "Error: sprite " << name << " not found!" << std::endl;
		m_totalFrames = 0;
	}
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