#pragma once
#include "Bullet.h"
#include "../defender/DefenderManager.h"

class LasertronBullet : public Bullet
{
public:
	LasertronBullet(Vector2 position);

	void update(float dt, Game& game) override;
	void draw(Game& game) override;
private:
};