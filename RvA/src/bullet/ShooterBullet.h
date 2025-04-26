#pragma once
#include "Bullet.h"
#include "../defender/DefenderManager.h"

class ShooterBullet : public Bullet
{
public:
	ShooterBullet(Vector2 position);

	void update(float dt, Game& game) override;
	void draw(Game& game) override;

	float getRadius() const { return m_radius; }	
private:
	float m_speed;
	float m_radius;
};