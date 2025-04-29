#include "Enemy.h"
#include "Game.h"
#include "defender/Defender.h"

std::string Enemy::getEnemyTypeName(EnemyType type)
{
    switch (type)
    {
    case EnemyType::B1: return "b1_alien_walk";
    case EnemyType::B2: return "b2_alien_walk";
    default: return "";
    }
}

void Enemy::takeDamage(int damage)
{
	m_hp -= damage;
}

void Enemy::setTargetDefender(Defender* defender)
{
    m_targetDefender = defender;
}

Enemy::Enemy(Vector2 position, EnemyType type, Atlas& atlas, int row)
	: m_position(position), m_animation(getEnemyTypeName(type), 0.1f, atlas), m_row(row), 
    m_attackTime(0.5f), m_targetDefender(nullptr), m_damage(50)
{
    switch (type)
    {
    case EnemyType::B1:
        m_maxHp = 100;
        m_speed = 40.f;
        break;
    case EnemyType::B2:
        m_maxHp = 150;
        m_speed = 80.f;
        break;
    }
	m_hp = m_maxHp;
	m_name = getEnemyTypeName(type);
}

void Enemy::update(float dt)
{
    if (m_targetDefender)
    {
		m_attackTime -= dt;

        if (m_attackTime <= 0.f)
        {
            m_targetDefender->takeDamage(m_damage);

            m_attackTime = 0.5f;
        }

        if (m_targetDefender->getHp() <= 0.f)
        {
            m_targetDefender->setAlive(false);
            m_targetDefender = nullptr;
        }
    }
    else
    {
        m_position.x -= m_speed * dt;
    }

	m_animation.update(dt);
}

void Enemy::draw(Game& game)
{
	game.getAtlas().drawAnimation(m_name.c_str(), m_position, m_animation.getCurrentFrame());
	game.getGUI().drawHp(m_hp, m_maxHp, m_position);
}