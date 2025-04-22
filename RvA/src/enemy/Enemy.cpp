#include "Enemy.h"
#include "Game.h"

Enemy::Enemy(Vector2 position, std::string name, Atlas& atlas)
	: m_position(position), m_speed(20.0f), m_name(name), m_animation(name, 0.2f, atlas)
{
}

void Enemy::update(float dt)
{
    m_position.x -= m_speed * dt;
	m_animation.update(dt);
}

void Enemy::draw(Game& game)
{
	game.getAtlas().drawAnimation(m_name.c_str(), m_position, m_animation.getCurrentFrame());
}