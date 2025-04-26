#pragma once
#include <raylib.h>
#include <string>
#include "EnemyTypes.h"
#include "Animation.h"

class Game;
class Atlas;
class Animation;

class Enemy
{
public:
    Enemy(Vector2 position, EnemyType type, Atlas& atlas);
        
    void update(float dt);
    void draw(Game& game, int cellSize);

    void takeDamage(int damage);
    int getHp() { return m_hp; }

    Vector2 getPosition() { return m_position; }
private:
	std::string m_name;
    Vector2 m_position;
    float m_speed;
    int m_hp;
	int m_maxHp;

	Animation m_animation;

    std::string getEnemyTypeName(EnemyType type);
};
