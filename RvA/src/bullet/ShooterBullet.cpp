#include "ShooterBullet.h"
#include "Game.h"

// TODO figure out how to pass cell size in a non-annoying way

ShooterBullet::ShooterBullet(Vector2 position)
	:Bullet(position), m_speed(150.0f), m_radius(5.0f), m_row(int(position.y) / 32 - 1)
{
}

void ShooterBullet::update(float dt, Game& game)
{
	m_position.x += m_speed * dt;

	if (m_position.x > game.getTexSize().x)
	{
		m_active = false;
	}
}

void ShooterBullet::draw(Game& game)
{
	DrawCircleV(m_position, m_radius, BLUE);
}