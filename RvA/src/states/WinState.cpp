#include "WinState.h"

#include "Game.h"
#include "constants.h"

#include <raylib.h>
#include <raymath.h>

WinState::WinState(Game& game) : m_game(game) {}

flow::FsmAction WinState::enter() {
	m_defenders.clear();
	auto solarPanelInfo = m_game.getGameRegistry().getDefender(DefenderType::Solar);
	m_defenders.push_back(createSpriteItem(solarPanelInfo->spriteEnabled.spriteInfo, {TEX_WIDTH, 100}, {-150, 0}));

	m_chasers.clear();
	auto x = TEX_WIDTH + 100.f;
	for (auto i = 0; i < 3; ++i) {
		const auto typeInfo = m_game.getGameRegistry().getEnemy("B1");
		m_chasers.push_back(createSpriteItem(typeInfo->moveAnimation.spriteInfo, {x, 100}, {-150, 0}));
		x += 30.f;
	}

	m_bullets.clear();
	for (auto i = 0; i < 4; ++i) {
		m_bullets.push_back(createSpriteItem(nullptr, {-100, 116}, {}));
	}

	m_drawingCallbackHandle = m_game.registerDrawingCallback([this]() {
		drawSprites(m_defenders);
		drawSprites(m_chasers);
		drawBullets(m_bullets);
	});

	m_game.getGameSession().end();

	auto& gui = m_game.getGUI();
	gui.buildScreen("Win")
		.vertical_stack(5, 200.f)
		.medium_text({.text = "You Won!!!", .color = WHITE, .hAlign = HAlign::Center})
		.space({0, 35.f})
		.small_text({.text = "Press any key to continue", .color = WHITE, .hAlign = HAlign::Center})
		.end();

	return flow::FsmAction::none();
}

void WinState::exit() {
	m_drawingCallbackHandle = {};
	m_game.getGUI().destroyScreen("Win");
}

flow::FsmAction WinState::update(float dt) {
	if (GetKeyPressed() || IsMouseButtonDown(MOUSE_LEFT_BUTTON) || IsMouseButtonDown(MOUSE_RIGHT_BUTTON) || IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
		return flow::FsmAction::transition("menu");
	}

	updateSprites(m_defenders, dt);
	updateSprites(m_chasers, dt);
	updateSprites(m_bullets, dt);

	// When the last chaser reach x position -100 we let them run away and start spawning the bullets
	if (m_chasers.back().position.x <= -100) {
		auto x = -20.f;
		for (auto& sprite : m_chasers) {
			sprite.velocity.x *= -1;
			sprite.position.x = x;
			sprite.flip = Flip::Horizontal;
			x -= 30;
		}

		x -= 50;
		for (auto& sprite : m_bullets) {
			sprite.velocity.x = 160;
			sprite.position.x = x;
			x -= 25;
		}
	}

	m_textAnimationTime += dt;
	m_textPosition.y = sinf(m_textAnimationTime * m_textAnimationSpeed) * m_textAnimationAmplitude;

	return flow::FsmAction::none();
}

WinState::SpriteItem WinState::createSpriteItem(const SpriteInfo* spriteInfo, const Vector2& position, const Vector2& velocity) {
	return {spriteInfo, position, velocity, spriteInfo ? std::make_unique<Animation>(Animation::createAnimation({spriteInfo, 0.05f})) : nullptr};
}

void WinState::updateSprites(std::vector<SpriteItem>& sprites, float dt) {
	for (auto& sprite : sprites) {
		auto offset = Vector2Scale(sprite.velocity, dt);
		sprite.position += offset;

		if (sprite.animation) {
			sprite.animation->update(dt);
		}
	}
}

void WinState::drawSprites(std::vector<SpriteItem>& sprites) {
	for (auto& sprite : sprites) {
		if (sprite.animation) {
			m_game.getAtlas().drawSprite(sprite.spriteInfo, sprite.position, sprite.animation->getCurrentFrame(), sprite.flip);
		}
	}
}

void WinState::drawBullets(std::vector<SpriteItem>& sprites) {
	for (auto& sprite : sprites) {
		DrawCircleV(sprite.position, 5.f, BLUE);
	}
}
