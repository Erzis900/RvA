#pragma once

#include "textureatlas.h"
#include <raylib.h>

class Atlas
{
public:
	Atlas();
	~Atlas();

	void load(const char* textureName);
	void drawSprite(const char* name, Vector2 pos);

	int getFrameSize() { return m_frameSize; }
private:
	texture_atlas_t* m_ta;
	Texture2D m_texture;

	int m_frameSize;
};