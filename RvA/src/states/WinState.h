#pragma once

#include "IGameState.h"
#include "Animation.h"
#include <vector>
#include <memory>

class WinState : public IGameState
{
public:
	WinState(Game& game);
	void draw(Game& game) override;
	void update(Game& game, float dt) override;

private:
	struct SpriteItem {
		std::string name;
		Vector2 position;
		Vector2 velocity;
		std::unique_ptr<Animation> animation;
		Flip flip{};
	};

	SpriteItem createSpriteItem(Game& game, const char* name, const Vector2& position, const Vector2& velocity);
	void updateSprites(std::vector<SpriteItem>& sprites, float dt);
	void drawSprites(std::vector<SpriteItem>& sprites, Game& game);
	void drawBullets(std::vector<SpriteItem>& sprites, Game& game);

	std::vector<SpriteItem> m_defenders;
	std::vector<SpriteItem> m_chasers;
	std::vector<SpriteItem> m_bullets;

	float m_textAnimationTime{};
	float m_TextAnimationSpeed{2.5f};
	float m_TextAnimationAmplitude{10.f};
	Vector2 m_textPosition;
};
