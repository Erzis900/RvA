#pragma once
#include <raylib.h>
#include <string>
#include "EnemyTypes.h"
#include "Animation.h"
#include "Damage.h"
#include "utilities.h"

class Game;
class Atlas;
class Animation;

enum class EnemyState
{
    Idle,
    Moving,
    DamageTaken,
    PrepareToAttack,
    ReadyToAttack,
    Dying,
    Dead
};

struct EnemyTypeInfo
{
    EnemyType type{};
    float spawnChance{};
    float maxHp{};
    float speed{};
    float attackTime{};
    float damage{};
    AnimationData idleAnimation;
    AnimationData moveAnimation;
    AnimationData attackAnimation;
    AnimationData dyingAnimation;
};

class Enemy
{
public:
    Enemy(Vector2 position, const EnemyTypeInfo* typeInfo, Atlas& atlas, int row);
        
    void update(float dt);
    void draw(Game& game);

    void applyDamage(const Damage& damage);

    auto getHp() const { return m_hp; }
    auto getRow() const { return m_row; }
    float getDamage() const;
    const Vector2& getPosition() const { return m_position; }
    Vector2 getCenteredPosition() const;
    Rectangle getBoundingBox() const;

    void setState(EnemyState state);
    auto getState() const { return m_state; }

    bool isDying() const;
    bool isAttacking() const;

private:
    void performIdle(float dt);
    void performDamageTaken(float dt);
    void performMove(float dt);
    void performPrepareAttack(float dt);
    void performDying(float dt);

    void setAnimation(const AnimationData& animationData);

	std::string m_spriteName;
    Vector2 m_position{};
    float m_hp{};
    float m_attackTime{};
    int m_row{};
    Interpolation<> m_damageTakenAnimation;
    Color m_tint{ WHITE };
    Damage m_latestDamageApplied;

	Animation m_animation;
    EnemyState m_state;
    Atlas& m_atlas;
    const EnemyTypeInfo* m_typeInfo{};
};
