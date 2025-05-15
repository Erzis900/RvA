#include "Atlas.h"

#include <iostream>

Atlas::Atlas() {
	if (texture_atlas_load("assets/atlas.map", &m_ta)) {
		std::cout << "atlas.map failed to load" << std::endl;
		return;
	}
}

Atlas::~Atlas() {
	UnloadTexture(m_texture);
	texture_atlas_free(m_ta);
}

void Atlas::load(const char* textureName) {
	m_texture = LoadTexture(textureName);
}

void Atlas::drawSprite(const SpriteInfo* spriteInfo, const Vector2& pos, int currentFrame, Flip flip, Color tint, float rotation) {
	drawSprite(spriteInfo, pos, {float(spriteInfo->frames[currentFrame].width), float(spriteInfo->frames[currentFrame].height)}, currentFrame, flip, tint, rotation);
}

void Atlas::drawSprite(const SpriteInfo* spriteInfo, const Vector2& pos, const Vector2& size, int currentFrame, Flip flip, Color tint, float rotation) {
	texture_atlas_frame_t* frameInfo = &spriteInfo->frames[currentFrame];

	Rectangle src = {float(frameInfo->x), float(frameInfo->y), float(frameInfo->width), float(frameInfo->height)};

	src.width = frameInfo->width * (1 - 2.f * ((flip & Flip::Horizontal) != 0));
	src.height = frameInfo->height * (1 - 2.f * ((flip & Flip::Vertical) != 0));

	DrawTexturePro(m_texture, src, {pos.x, pos.y, size.x, size.y}, {0.f, 0.f}, rotation, tint);
}

const texture_atlas_texture_t* Atlas::getSpriteInfo(const char* textureName) const {
	return texture_atlas_lookup(m_ta, textureName);
}
