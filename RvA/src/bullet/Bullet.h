#pragma once
#include <raylib.h>

class Game;

class Bullet {
public:
    Bullet(Vector2 position) : m_position(position), m_active(true) {}
    virtual ~Bullet() = default;

    virtual void update(float dt, Game& game) = 0;
    virtual void draw(Game& game) = 0;

    bool isActive() const { return m_active; }
	Vector2 getPosition() const { return m_position; }
	void setActive(bool active) { m_active = active; }

protected:
    Vector2 m_position;
    bool m_active;
};
