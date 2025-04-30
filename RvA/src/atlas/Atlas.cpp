#include "Atlas.h"
#include <iostream>

Atlas::Atlas()
{
	if (texture_atlas_load("assets/atlas.map", &m_ta))
	{
		std::cout << "atlas.map failed to load" << std::endl;
		return;
	}
}

Atlas::~Atlas()
{
	UnloadTexture(m_texture);
	texture_atlas_free(m_ta);
}

void Atlas::load(const char* textureName)
{
	m_texture = LoadTexture(textureName);
}

void Atlas::drawSprite(const char* name, Vector2 pos)
{
	texture_atlas_texture_t* textureInfo = texture_atlas_lookup(m_ta, name);
	if (textureInfo)
	{
		Rectangle src = { 
			float(textureInfo->frames->x), 
			float(textureInfo->frames->y), 
			float(textureInfo->frames->width), 
			float(textureInfo->frames->height) 
		};

		DrawTexturePro(m_texture, src, {pos.x, pos.y, float(textureInfo->frames->width), float(textureInfo->frames->height)}, {0.f, 0.f}, 0.f, WHITE);
	}
	else
	{
		std::cout << "Error: sprite " << name << " not found!" << std::endl;
		return;
	}
}

void Atlas::drawAnimation(const char* name, Vector2 pos, int currentFrame, Flip flip)
{
	texture_atlas_texture_t* textureInfo = texture_atlas_lookup(m_ta, name);
	if (textureInfo)
	{
		texture_atlas_frame_t* frameInfo = textureInfo->frames + currentFrame;

		Rectangle src = {
			float(frameInfo->x),
			float(frameInfo->y),
			float(frameInfo->width),
			float(frameInfo->height)
		};

		src.width = frameInfo->width * (1 - 2.f * ((flip & Flip::Horizontal) != 0));
		src.height = frameInfo->height * (1 - 2.f * ((flip & Flip::Vertical) != 0));

		DrawTexturePro(m_texture, src, { pos.x, pos.y, float(frameInfo->width), float(frameInfo->height) }, { 0.f, 0.f }, 0.f, WHITE);
	}
	else
	{
		std::cout << "Error: sprite " << name << " not found!" << std::endl;
		return;
	}
}