#pragma once
#include <raylib.h>
#include <string>
#include "DefenderTypes.h"
#include "Animation.h"

class Game;
class Atlas;

class Defender
{
public:
    Defender(Vector2 position, int row, int col, DefenderType type, Atlas& atlas);

    void update(float dt, float& energy);
    void draw(Game& game, int cellSize);

    Vector2 getPosition() const { return m_position; }
    DefenderType getType() const { return m_type; }
	int getRow() const { return m_row; }
	int getCol() const { return m_col; }

	void setActive(bool active) { m_active = active; }
	bool isActive() const { return m_active; }
private:
    Vector2 m_position;
    DefenderType m_type;

    std::string m_name;
    std::string m_offName;
    Animation m_animation;

    int m_row;
    int m_col;

    int m_hp;
    int m_maxHp;

    float m_energyDelta;
    bool m_active;

    std::string getDefenderTypeName(DefenderType type);
    void updateEnergy(float dt, float& energy);
};
