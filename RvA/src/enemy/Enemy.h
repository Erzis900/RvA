#pragma once
#include <raylib.h>
#include <string>
#include "EnemyTypes.h"
#include "Animation.h"
#include <memory>

class Game;
class Atlas;
class Animation;

class Enemy
{
public:
    Enemy(Vector2 position, EnemyType type, Atlas& atlas, int row);
        
    void update(float dt);
    void draw(Game& game);

    void takeDamage(int damage);

    int getHp() { return m_hp; }
	int getRow() { return m_row; }
    int getDamage() const;
    const Vector2& getPosition() const { return m_position; }

    static const char* getEnemyTypeName(EnemyType type);

    enum class AttackState
    {
        NoAttack,
        PrepareToAttack,
        ReadyToAttack
    };

    void setAttackState(AttackState attackState);
    AttackState getAttackState() const;

private:
	std::string m_name;
    Vector2 m_position;
    float m_speed;
    int m_hp;
	int m_maxHp;
    int m_row;

    float m_attackTime;
    int m_damage;

	Animation m_animation;
    AttackState m_attackState{AttackState::NoAttack};
};
