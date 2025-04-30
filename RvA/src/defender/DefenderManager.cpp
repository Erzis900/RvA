#include "DefenderManager.h"
#include "Game.h"
#include <iostream>
#include "enemy/enemyManager.h"
#include "bullet/ShooterBullet.h"

DefenderManager::DefenderManager(Game& game)
	: m_game(game)
{
}

void DefenderManager::update(float dt, float& energy, float &batteries, EnemyManager& enemyManager)
{
    for (auto& defender : m_defenders)
    {
        defender->update(dt, energy, batteries);

        for (auto& enemy : enemyManager.getEnemies())
        {
			if (defender->getRow() == enemy->getRow())
			{
                if (enemy->getPosition().x <= defender->getPosition().x + CELL_SIZE && enemy->getPosition().x > defender->getPosition().x)
                {
                    enemy->setTargetDefender(defender.get());
                }   
			}
        }

		if (!defender->isAlive())
		{
			m_occupied[defender->getRow()][defender->getCol()] = false;
		}
    }

    for (auto& bullet : m_bullets)
    {
		if (!bullet->isActive()) continue;

        bullet->update(dt, m_game);

		for (auto& enemy : enemyManager.getEnemies())
		{
            if (ShooterBullet* shooterBullet = dynamic_cast<ShooterBullet*>(bullet.get()))
            {
                if (enemy->getRow() == shooterBullet->getRow())
                {
                    if (bullet->getPosition().x > enemy->getPosition().x)
                    {
                        bullet->setActive(false);
                        enemy->takeDamage(50);
                    }
                }
            }
		}
    }

    m_bullets.erase(std::remove_if(m_bullets.begin(), m_bullets.end(),
        [](const std::unique_ptr<Bullet>& b) { return !b->isActive(); }),
        m_bullets.end());

    m_defenders.erase(
        std::remove_if(m_defenders.begin(), m_defenders.end(),
            [](const std::unique_ptr<Defender>& defender)
            {
                return defender->getHp() <= 0.f;

            }),
        m_defenders.end()
    );

    handlePlace(batteries);
}

void DefenderManager::draw()
{
    for (auto& defender : m_defenders)
    {
        defender->draw(m_game);
    }

    for (auto& bullet : m_bullets)
    {
        bullet->draw(m_game);
    }
}

void DefenderManager::spawnBullet(std::unique_ptr<Bullet> bullet)
{
    m_bullets.push_back(std::move(bullet));
}

void DefenderManager::handlePlace(float& batteries)
{
    if ((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)))
    {
        Vector2 mousePos = GetMousePosition();

        if (mousePos.x > CELL_SIZE && mousePos.y > CELL_SIZE && mousePos.x < TEX_WIDTH - CELL_SIZE &&
            mousePos.y < TEX_HEIGHT - (TEX_HEIGHT - (ROWS + 1) * CELL_SIZE))
        {
            int row = int(mousePos.y) / CELL_SIZE - 1;
            int col = int(mousePos.x) / CELL_SIZE - 1;

            DefenderType type = m_game.getGUI().getSelectedDefender();
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && type != DefenderType::None)
            {
                if (!m_occupied[row][col])
                {
                    float x = float(col * CELL_SIZE + CELL_SIZE);
                    float y = float(row * CELL_SIZE + CELL_SIZE);

                    int cost = m_costs[static_cast<int>(type)];

                    if (cost <= batteries)
                    {
                        m_defenders.push_back(std::make_unique<Defender>(Vector2{ x, y }, row, col, cost, type, m_game, *this));
                        m_occupied[row][col] = true;
                        batteries -= cost;
                    }
				}
			}
			else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
			{
                if (m_occupied[row][col])
                {
                    for (auto& defender : m_defenders)
                    {
                        if (defender->getRow() == row && defender->getCol() == col)
                        {
                            defender->setActive(!defender->isActive());
                            break;
                        }
                    }
                }
            }
        }
    }
}

void DefenderTypeRegistry::registerDefender(DefenderTypeInfo defenderTypeInfo)
{
    m_defenderTypes.insert({ defenderTypeInfo.type, std::move(defenderTypeInfo) });
}

const DefenderTypeInfo* DefenderTypeRegistry::getDefenderInfo(DefenderType type) const
{
    auto itr = m_defenderTypes.find(type);
    return (itr != m_defenderTypes.end()) ? &itr->second : nullptr;
}

DefenderManager2::DefenderManager2(Atlas& atlas, GUI& gui)
    : m_atlas(atlas)
    , m_gui(gui)
{
    m_defenders.reserve(128);
}

void DefenderManager2::draw()
{
    for (auto& defender : m_defenders)
    {
        m_atlas.drawAnimation(
            defender->isActive ? defender->info->spriteEnabled.c_str() : defender->info->spriteDisabled.c_str(),
            defender->position,
            defender->isActive ? defender->animation.getCurrentFrame() : 0);

        m_gui.drawHp(defender->hp, defender->info->maxHP, defender->position);
    }
}

DefenderUpdateResult DefenderManager2::update(float dt)
{
    DefenderUpdateResult result;
    for (auto it = m_defenders.begin(); it != m_defenders.end(); )
    {
        auto& defender = *it;

        defender->animation.update(dt);
        if (defender->isActive)
        {
            result.amountOfEnergyDrain += dt * defender->info->energyDrain;
            
            if(defender->info->batteryGain != 0) 
            {
                defender->batteryGainTime += dt;
                if (defender->batteryGainTime > 1.f)
                {
                    defender->batteryGainTime = 0;
                    result.amountOfBatteryGain += defender->info->batteryGain;
                }
            }
            
            if (defender->info->bulletType)
            {
                defender->shootTime -= dt;
                if (defender->shootTime <= 0)
                {
                    defender->shootTime = defender->info->shootCooldown;
                    result.actions.push_back(BulletSpawnAction{
                        .bulletType = *defender->info->bulletType,
                        .position = defender->position
                    });
                }
            }
        }

        if (defender->hp <= 0)
        {
            m_defenderGrid[defender->row][defender->column] = nullptr;
            it = m_defenders.erase(it);
        }
        else
        {
            ++it;
        }
    }
    return result;
}

const std::vector<std::unique_ptr<Defender2>>& DefenderManager2::getDefenders() const
{
    return m_defenders;
}

void DefenderManager2::spawnDefender(const DefenderTypeInfo* defenderTypeInfo, int row, int column)
{
    auto defender = std::make_unique<Defender2>();
    defender->info = defenderTypeInfo;
    defender->shootTime = defenderTypeInfo->firstShootCooldown;
    defender->position = Vector2{ float(column) * CELL_SIZE + CELL_SIZE, float(row) * CELL_SIZE + CELL_SIZE };
    defender->isActive = false;
    defender->column = column;
    defender->row = row;
    defender->animation = Animation::createAnimation(defenderTypeInfo->spriteEnabled.c_str(), 0.1f, m_atlas);
    defender->hp = defender->info->maxHP;
    m_defenderGrid[row][column] = defender.get();
    m_defenders.push_back(std::move(defender));
}

void DefenderManager2::toggleDefender(int row, int column)
{
    auto defender = m_defenderGrid[row][column];
    if (defender)
    {
        defender->isActive = !defender->isActive;
    }
}

bool DefenderManager2::hasDefender(int row, int column) const
{
    auto defender = m_defenderGrid[row][column];
    return defender != nullptr;
}
