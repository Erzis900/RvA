#pragma once

#include <vector>
#include <optional>
#include <functional>

#include "defender/DefenderTypes.h"
#include "atlas/Atlas.h"
#include "utilities/CallbackRegistry.h"

class GUI;
class Atlas;

struct HUDDefenderData
{
	DefenderType type;
	const SpriteInfo* spriteInfo;
	int cost;
};

struct ProgressBarData
{
	float value{};
	float max{};
	Vector2 position{};
	Color bkgColor{};
	Color fillColor{};
};

struct HUDData
{
	float batteryCharge{};
	int scrapsAmount{};
	int numberOfEnemiesDefeated{};
	int numberOfEnemiesToDefeat{};
	std::vector<HUDDefenderData> defenders;
	std::vector<ProgressBarData> progressBars;

	std::optional<DefenderType> selectedDefender;
};

class HUD
{
public:
	HUD(GUI& gui);

	void setEnable(bool enabled);
	void draw(Atlas& atlas);

	// Events
	CallbackHandle onDefenderSelected(std::function<void()> callback);

	// Data
	auto& data() { return m_data; }
	void clear();

private:
	void drawScrapAmount();
	void drawBatteryCharge();
	void drawDefenders(Atlas& atlas);
	void drawProgressBars();
	void drawProgressBar(float value, float max, const Vector2& pos, Color bkgColor = DARKGRAY, Color fillColor = GREEN);


	bool m_defenderHover{};
    bool m_isEnabled{ true };
	HUDData m_data;
	GUI& m_gui;

	CallbackRegistry<> m_onDefenderSelectedCallbacks;
};