#pragma once

#include "textureatlas.h"
#include <raylib.h>
#include <string>

class Atlas
{
public:
	Atlas();
	~Atlas();

	void load(const char* textureName);
	void drawSprite(const char* name, Vector2 pos);
	void drawAnimation(const char* name, Vector2 pos, int currentFrame);

	texture_atlas_t* getTextureAtlas() { return m_ta; }
private:
	texture_atlas_t* m_ta;
	Texture2D m_texture;
};