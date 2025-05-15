#pragma once

#include "textureatlas.h"

#include <raylib.h>
#include <string>

enum Flip {
	None = 0,
	Vertical = 1 << 0,
	Horizontal = 1 << 1
};

using SpriteInfo = texture_atlas_texture_t;

class Atlas {
public:
	Atlas();
	~Atlas();

	void load(const char* textureName);
	void drawSprite(const SpriteInfo* spriteInfo, const Vector2& pos, int currentFrame = 0, Flip flip = Flip::None, Color tint = WHITE, float rotation = 0.f);
	void drawSprite(const SpriteInfo* spriteInfo, const Vector2& pos, const Vector2& size, int currentFrame = 0, Flip flip = Flip::None, Color tint = WHITE, float rotation = 0.f);
	void drawRotatedSprite(const SpriteInfo* spriteInfo, const Vector2& pos, int currentFrame, Flip flip, Color tint, float rotation);
	void drawRotatedSprite(const SpriteInfo* spriteInfo, const Vector2& pos, const Vector2& size, int currentFrame, Flip flip, Color tint, float rotation);


	texture_atlas_t* getTextureAtlas() {
		return m_ta;
	}

	const SpriteInfo* getSpriteInfo(const char* textureName) const;

private:
	texture_atlas_t* m_ta;
	Texture2D m_texture;
};
