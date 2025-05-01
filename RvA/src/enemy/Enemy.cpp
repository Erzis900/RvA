#include "Enemy.h"
#include "Game.h"
#include <raymath.h>

Enemy::Enemy(Vector2 position, const EnemyTypeInfo* typeInfo, Atlas& atlas, int row)
    : m_position(position), m_row(row), m_atlas(atlas), m_typeInfo(typeInfo)
{
    m_hp = typeInfo->maxHp;
    m_attackTime = typeInfo->attackTime;
    setState(EnemyState::Idle);
}

void Enemy::setState(EnemyState state)
{
    if (m_state != state)
    {
        m_state = state;

        switch (state)
        {
        case EnemyState::Idle:
            setAnimation(m_typeInfo->idleAnimation);
            break;
        case EnemyState::Moving:
            setAnimation(m_typeInfo->moveAnimation);
            break;
        case EnemyState::PrepareToAttack:
        case EnemyState::ReadyToAttack:
            setAnimation(m_typeInfo->attackAnimation);
            m_attackTime = 0.5f;
            break;
        case EnemyState::Dying:
            setAnimation(m_typeInfo->dyingAnimation);
            break;
        }
    }
}

bool Enemy::isDying() const
{
    return getState() == EnemyState::Dying || getState() == EnemyState::Dead;
}

void Enemy::takeDamage(float damage)
{
	m_hp -= damage;
}

void Enemy::update(float dt)
{
    switch (m_state)
    {
    case EnemyState::Idle:
        performIdle(dt);
        break;
    case EnemyState::Moving:
        performMove(dt);
        break;
    case EnemyState::PrepareToAttack:
        performPrepareAttack(dt);
        break;
    case EnemyState::Dying:
        performDying(dt);
        break;
    case EnemyState::Dead:
        break;
    }

    if (m_hp <= 0 && !isDying())
    {
        setState(EnemyState::Dying);
    }

	m_animation.update(dt);
}

void Enemy::draw(Game& game)
{
	game.getAtlas().drawSprite(m_animation.getSpriteInfo(), m_position, m_animation.getCurrentFrame());

    if (!isDying())
    {
        game.getGUI().drawHp(m_hp, m_typeInfo->maxHp, m_position);
    }
}

float Enemy::getDamage() const
{
    return m_typeInfo->damage;
}

Vector2 Enemy::getCenteredPosition() const
{
    return Vector2Add(m_position, { 16, 16 });
}

Rectangle Enemy::getBoundingBox() const
{
    const float bbPadding = 5;
    return Rectangle{ m_position.x + bbPadding, m_position.y + bbPadding, 32 - bbPadding, 32 - bbPadding };
}

void Enemy::performIdle(float dt)
{
    // We immediately change state to Move for now
    setState(EnemyState::Moving);
}

void Enemy::performMove(float dt)
{
    m_position.x -= m_typeInfo->speed * dt;
}

void Enemy::performPrepareAttack(float dt)
{
    m_attackTime -= dt;
    if (m_attackTime <= 0.f)
    {
        setState(EnemyState::ReadyToAttack);
    }
}

void Enemy::performDying(float dt)
{
    if (m_animation.isOver())
    {
        setState(EnemyState::Dead);
    }
}

void Enemy::setAnimation(const AnimationData& animationData)
{
    m_animation = Animation::createAnimation(animationData);
}
