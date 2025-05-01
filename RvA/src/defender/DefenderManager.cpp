#include "DefenderManager.h"
#include "Game.h"

void DefenderTypeRegistry::registerDefender(DefenderTypeInfo defenderTypeInfo)
{
    m_defenderTypes.insert({ defenderTypeInfo.type, std::move(defenderTypeInfo) });
}

const DefenderTypeInfo* DefenderTypeRegistry::getDefenderInfo(DefenderType type) const
{
    auto itr = m_defenderTypes.find(type);
    return (itr != m_defenderTypes.end()) ? &itr->second : nullptr;
}

DefenderManager::DefenderManager(Atlas& atlas, GUI& gui)
    : m_atlas(atlas)
    , m_gui(gui)
{
    m_defenders.reserve(128);
}

void DefenderManager::draw()
{
    for (auto& defender : m_defenders)
    {
        m_atlas.drawSprite(
            defender->isActive ? defender->info->spriteEnabled.spriteInfo : defender->info->spriteDisabled.spriteInfo,
            defender->position,
            defender->isActive ? defender->animation.getCurrentFrame() : 0);

        m_gui.drawHp(defender->hp, defender->info->maxHP, defender->position);
    }
}

DefenderUpdateResult DefenderManager::update(float dt)
{
    DefenderUpdateResult result;
    for (auto it = m_defenders.begin(); it != m_defenders.end(); )
    {
        auto& defender = *it;

        defender->animation.update(dt);
        if (defender->isActive)
        {
            result.amountOfBatteryDrain += dt * defender->info->batteryDrain;
            
            if(defender->info->scrapsGain != 0) 
            {
                defender->scrapsGainTime += dt;
                if (defender->scrapsGainTime > 1.f)
                {
                    defender->scrapsGainTime = 0;
                    result.amountOfScrapsGain += defender->info->scrapsGain;
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

const std::vector<std::unique_ptr<Defender>>& DefenderManager::getDefenders() const
{
    return m_defenders;
}

void DefenderManager::spawnDefender(const DefenderTypeInfo* defenderTypeInfo, int row, int column)
{
    auto defender = std::make_unique<Defender>();
    defender->info = defenderTypeInfo;
    defender->shootTime = defenderTypeInfo->firstShootCooldown;
    defender->position = Vector2{ float(column) * CELL_SIZE + CELL_SIZE, float(row) * CELL_SIZE + CELL_SIZE };
    defender->isActive = false;
    defender->column = column;
    defender->row = row;
    defender->animation = Animation::createAnimation(defenderTypeInfo->spriteEnabled);
    defender->hp = defender->info->maxHP;
    m_defenderGrid[row][column] = defender.get();
    m_defenders.push_back(std::move(defender));
}

void DefenderManager::toggleDefender(int row, int column)
{
    auto defender = m_defenderGrid[row][column];
    if (defender)
    {
        defender->isActive = !defender->isActive;
    }
}

bool DefenderManager::hasDefender(int row, int column) const
{
    auto defender = m_defenderGrid[row][column];
    return defender != nullptr;
}
