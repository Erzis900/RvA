#include "Atlas.h"
#include <iostream>

Atlas::Atlas()
{
	if (texture_atlas_load("assets/atlas.map", &m_ta))
	{
		std::cout << "atlas.map failed to load" << std::endl;
		return;
	}
	
	m_frameSize = 32;
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
		Rectangle src = { float(textureInfo->frames->x), float(textureInfo->frames->y), float(textureInfo->frames->width), float(textureInfo->frames->height) };
		{
			DrawTexturePro(m_texture, src, {pos.x, pos.y, float(textureInfo->frames->width), float(textureInfo->frames->height)}, {0.f, 0.f}, 0.f, WHITE);
		}
	}
	else
	{
		std::cout << "Error: sprite " << name << " not found!" << std::endl;
		return;
	}
}