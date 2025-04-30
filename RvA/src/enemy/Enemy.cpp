#include "Enemy.h"
#include "Game.h"

const char* Enemy::getEnemyTypeName(EnemyType type)
{
    switch (type)
    {
    case EnemyType::B1: return "b1_alien_walk";
    case EnemyType::B2: return "b2_alien_walk";
    default: return "";
    }
}

Enemy::Enemy(Vector2 position, EnemyType type, Atlas& atlas, int row)
    : m_position(position), m_animation(Animation::createAnimation(getEnemyTypeName(type), 0.1f, atlas)), m_row(row),
    m_attackTime(0.5f), m_damage(50)
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

void Enemy::setAttackState(AttackState attackState)
{
    if (m_attackState != attackState)
    {
        switch (attackState)
        {
        case AttackState::NoAttack:
        case AttackState::PrepareToAttack: 
            m_attackTime = 0.5f;
            break;
        }
    }

    m_attackState = attackState;

}

Enemy::AttackState Enemy::getAttackState() const
{
    return m_attackState;
}

void Enemy::takeDamage(int damage)
{
	m_hp -= damage;
}

void Enemy::update(float dt)
{
    if (m_attackState == AttackState::PrepareToAttack)
    {
        m_attackTime -= dt;
        if (m_attackTime <= 0.f)
        {
            m_attackState = AttackState::ReadyToAttack;
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

int Enemy::getDamage() const
{
    return m_damage;
}