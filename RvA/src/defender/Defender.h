#pragma once
#include <raylib.h>
#include <string>
#include "DefenderTypes.h"
#include "Animation.h"

class Game;
class Atlas;
class DefenderManager;

class Defender
{
public:
    Defender(Vector2 position, int row, int col, int cost, DefenderType type, Game& game, DefenderManager& defenderManager);

    void update(float dt, float& energy, int &batteries);
    void draw(Game& game);

    void takeDamage(int damage);

	int getHp() const { return m_hp; }

    Vector2 getPosition() const { return m_position; }
    DefenderType getType() const { return m_type; }
	int getRow() const { return m_row; }
	int getCol() const { return m_col; }

	void setActive(bool active) { m_active = active; }
	void setAlive(bool alive) { m_isAlive = alive; }

	bool isActive() const { return m_active; }
	bool isAlive() const { return m_isAlive; }
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
    int m_batteryDelta;
    bool m_active;

    float m_shootCooldown;
    float m_shootTimer;

    bool m_isAlive;

    std::string getDefenderTypeName(DefenderType type);
    void updateEnergy(float dt, float& energy);
	void updateBatteries(float dt, int& batteries);

    Game& m_game;
    DefenderManager& m_defenderManager;
};
