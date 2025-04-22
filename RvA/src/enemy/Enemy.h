#pragma once
#include <raylib.h>
#include <string>
#include "Animation.h"

class Game;
class Atlas;

class Enemy
{
public:
    Enemy(Vector2 position, std::string name, Atlas& atlas);
        
    void update(float dt);
    void draw(Game& game);

    Vector2 getPosition() { return m_position; }
private:
    Vector2 m_position;
    float m_speed;

	std::string m_name;
	Animation m_animation;
};
