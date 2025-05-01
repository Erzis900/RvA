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

bool Enemy::isAttacking() const
{
    return getState() == EnemyState::ReadyToAttack || getState() == EnemyState::PrepareToAttack;
}

void Enemy::applyDamage(const Damage& damage)
{
    m_latestDamageApplied = damage;
    m_hp -= m_latestDamageApplied.value;
    m_damageTakenAnimation.start(0, 1, 0.25f)
        .onTick([this, position = m_position](const auto& value) {
            m_tint = colorLerp(RED, WHITE, value);
            if (m_latestDamageApplied.bounceBackPower != 0)
            {
                m_position = Vector2Add(position, Vector2Lerp({ 0, 0 }, { m_latestDamageApplied.bounceBackPower, 0 }, value));
            }
        })
        .onComplete([this]() {
            m_tint = WHITE;
            m_latestDamageApplied = {};
        });
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
    case EnemyState::DamageTaken:
        performDamageTaken(dt);
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
    m_damageTakenAnimation.update(dt);
}

void Enemy::draw(Game& game)
{
	game.getAtlas().drawSprite(m_animation.getSpriteInfo(), m_position, m_animation.getCurrentFrame(), Flip::None, m_tint);

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
    constexpr Vector2 bbPadding = { 10, 5 };
    return Rectangle{ m_position.x + bbPadding.x, m_position.y + bbPadding.y, 32 - bbPadding.x, 32 - bbPadding.y };
}

void Enemy::performIdle(float dt)
{
    // We immediately change state to Move for now
    setState(EnemyState::Moving);
}

void Enemy::performDamageTaken(float dt)
{
    m_damageTakenAnimation.update(dt);
    if (m_latestDamageApplied.bounceBackPower == 0) {
        performMove(dt);
    }
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
