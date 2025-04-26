#include "Defender.h"
#include "Game.h"
#include <iostream>
#include "states/LostState.h"
#include <memory>
#include "../bullet/ShooterBullet.h"
#include "DefenderManager.h"

std::string Defender::getDefenderTypeName(DefenderType type)
{
    switch (type)
    {
    case DefenderType::Solar:     return "solar_idle";
    case DefenderType::Shooter:   return "shooter_idle";
    case DefenderType::Catapult:  return "catapult_idle";
    case DefenderType::Lasertron: return "lasertron_idle";
    default: return "";
    }
}

Defender::Defender(Vector2 position, int row, int col, int cost, DefenderType type, Game& game, DefenderManager& defenderManager)
    : m_position(position), m_type(type), m_name(getDefenderTypeName(type)), m_animation(m_name, 0.1f, game.getAtlas()),
	m_row(row), m_col(col), m_hp(0), m_maxHp(0), m_energyDelta(0.f), m_active(false), m_batteryDelta(0), m_game(game),
	m_shootCooldown(1.f), m_shootTimer(0.f), m_defenderManager(defenderManager)
{
	std::string baseName = m_name.substr(0, m_name.find("_idle"));
	m_offName = baseName + "_off";

	switch (type)
	{
	case DefenderType::Solar:
		m_maxHp = 100;
		m_energyDelta = 5.f;
		m_batteryDelta = 5;
		break;
	case DefenderType::Shooter:
		m_maxHp = 150;
		m_energyDelta = -5.f;
		break;
	case DefenderType::Catapult:
		m_maxHp = 200;
		m_energyDelta = -10.f;
		break;
	case DefenderType::Lasertron:
		m_maxHp = 250;
		m_energyDelta = -20.f;
		break;
	}
	m_hp = m_maxHp;
}

void Defender::updateEnergy(float dt, float& energy)
{
	if (m_active)
	{
		energy += dt * m_energyDelta;

		if (energy > 100.f) energy = 100.f;
		if (energy < 0.f)
		{
			m_game.setState(std::make_unique<LostState>(m_game));
		}
	}
}

void Defender::updateBatteries(float dt, int& batteries)
{
	if (m_active)
	{
		static float elapsedTime = 0.f;
		elapsedTime += dt;

		if (elapsedTime >= 1.f)
		{
			elapsedTime = 0.f;
			batteries += m_batteryDelta;
		}
	}
}

void Defender::update(float dt, float& energy, int &batteries)
{
	m_animation.update(dt);
	updateEnergy(dt, energy);
	updateBatteries(dt, batteries);

	m_shootTimer += dt;

	if (m_type == DefenderType::Shooter && m_shootTimer >= m_shootCooldown && m_active)
	{
		m_shootTimer = 0.f;

		m_defenderManager.spawnBullet(
			std::make_unique<ShooterBullet>(Vector2{ m_position.x + 20, m_position.y + 20 })
		);
	}
}

void Defender::draw(Game& game, int cellSize)
{
	if (m_active)
	{
		game.getAtlas().drawAnimation(m_name.c_str(), m_position, m_animation.getCurrentFrame());
	}
	else 
	{
		game.getAtlas().drawSprite(m_offName.c_str(), m_position);
	}
    
	game.getGUI().drawHp(cellSize, m_hp, m_maxHp, m_position);
}