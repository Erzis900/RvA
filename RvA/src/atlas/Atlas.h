#pragma once

#include "textureatlas.h"
#include <raylib.h>
#include <string>

enum Flip {
	None = 0,
	Vertical = 1 << 0,
	Horizontal = 1 << 1
};

class Atlas
{
public:
	Atlas();
	~Atlas();

	void load(const char* textureName);
	void drawSprite(const char* name, Vector2 pos);
	void drawAnimation(const char* name, Vector2 pos, int currentFrame, Flip flip = Flip::None);

	texture_atlas_t* getTextureAtlas() { return m_ta; }
private:
	texture_atlas_t* m_ta;
	Texture2D m_texture;
};