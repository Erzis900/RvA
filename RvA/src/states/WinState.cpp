#include "WinState.h"
#include "MenuState.h"
#include "Game.h"
#include "enemy/Enemy.h"
#include "constants.h"
#include <raylib.h>
#include <raymath.h>

WinState::WinState(Game& game) {
	auto solarPanelInfo = game.getDefenderRegistry().getDefenderInfo(DefenderType::Solar);
	m_defenders.push_back(createSpriteItem(game, solarPanelInfo->spriteEnabled.c_str(), {TEX_WIDTH, 100}, {-150, 0}));

	auto x = TEX_WIDTH + 100.f;
	for (auto i = 0; i < 3; ++i)
	{
		const auto typeInfo = game.getEnemyTypeRegistry().getEnemyTypeInfo(EnemyType::B1);
		m_chasers.push_back(createSpriteItem(game, typeInfo->moveAnimation.animationName.c_str(), {x, 100}, {-150, 0}));
		x += 30.f;
	}

	for (auto i = 0; i < 4; ++i)
	{
		m_bullets.push_back(createSpriteItem(game, nullptr, { -100, 116 }, {}));
	}
}

void WinState::draw(Game& game)
{
	auto& guiHelper = game.getGUIHelper();
	guiHelper.DrawText({ "You Won!!!", 30, WHITE, { m_textPosition, GUIAlignmentH::Center, GUIAlignmentV::Center } });

	guiHelper.DrawText({ "Press any key to continue", 18, WHITE, { {0, 10}, GUIAlignmentH::Center, GUIAlignmentV::Bottom } });

	drawSprites(m_chasers, game);
	drawSprites(m_defenders, game);
	drawBullets(m_bullets, game);
}

void WinState::update(Game& game, float dt)
{
	if(
		GetKeyPressed() ||
		IsMouseButtonDown(MOUSE_LEFT_BUTTON) ||
		IsMouseButtonDown(MOUSE_RIGHT_BUTTON) ||
		IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
		game.setState(std::make_unique<MenuState>());
	}

	updateSprites(m_defenders, dt);
	updateSprites(m_chasers, dt);
	updateSprites(m_bullets, dt);

	// When the last chaser reach x position -100 we let them run away and start spawning the bullets
	if (m_chasers.back().position.x <= -100)
	{
		auto x = -20.f;
		for (auto& sprite : m_chasers) {
			sprite.velocity.x *= -1;
			sprite.position.x = x;
			sprite.flip = Flip::Horizontal;
			x -= 30;
		}

		x -= 50;
		for (auto& sprite : m_bullets)
		{
			sprite.velocity.x = 160;
			sprite.position.x = x;
			x -= 25;
		}
	}

	m_textAnimationTime += dt;
	m_textPosition.y = sinf(m_textAnimationTime * m_textAnimationSpeed) * m_textAnimationAmplitude;
}

WinState::SpriteItem WinState::createSpriteItem(Game& game, const char* name, const Vector2& position, const Vector2& velocity)
{
	return {
		name ? name : "",
		position,
		velocity,
		name ? std::make_unique<Animation>(Animation::createAnimation(name, 0.05f, game.getAtlas())) : nullptr
	};
}

void WinState::updateSprites(std::vector<SpriteItem>& sprites, float dt)
{
	for (auto& sprite : sprites) {
		auto offset = Vector2Scale(sprite.velocity, dt);
		sprite.position += offset;
		
		if (sprite.animation)
		{
			sprite.animation->update(dt);
		}
	}
}

void WinState::drawSprites(std::vector<SpriteItem>& sprites, Game& game)
{
	for (auto& sprite : sprites) {
		if (sprite.animation) {
			game.getAtlas().drawAnimation(sprite.name.c_str(), sprite.position, sprite.animation->getCurrentFrame(), sprite.flip);
		}
	}
}

void WinState::drawBullets(std::vector<SpriteItem>& sprites, Game& game)
{
	for (auto& sprite : sprites) {
		DrawCircleV(sprite.position, 5.f, BLUE);
	}
}
