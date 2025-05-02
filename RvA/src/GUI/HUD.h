#pragma once

#include <vector>
#include <optional>
#include <functional>
#include "defender/DefenderTypes.h"
#include "atlas/Atlas.h"

class GUI;
class Atlas;

struct HUDDefenderData
{
	DefenderType type;
	const SpriteInfo* spriteInfo;
	int cost;
};

struct HUDData
{
	float batteryCharge{};
	int scrapsAmount{};
	int numberOfEnemiesDefeated{};
	int numberOfEnemiesToDefeat{};
	bool drawPause{};
	std::vector<HUDDefenderData> defenders;

	std::optional<DefenderType> selectedDefender;
};

class HUD
{
public:
	HUD(Atlas& atlas, GUI& gui);

	void draw();
	void drawHPBar();

	// Events
	void onPauseButtonPressed(std::function<void()> callback);
	void onResumeButtonPressed(std::function<void()> callback);
	void onMenuButtonPressed(std::function<void()> callback);
	void onDefenderSelected(std::function<void()> callback);

	// Data
	auto& data() { return m_data; }

private:
	void drawScrapAmount();
	void drawBatteryCharge();
	void drawDefenders();
	void drawMenuButtons();
	void drawPause();

	bool m_defenderHover{};
	HUDData m_data;
	Atlas& m_atlas;
	GUI& m_gui;

	std::function<void()> m_onPauseButtonPressedCallback;
	std::function<void()> m_onMenuButtonPressedCallback;
	std::function<void()> m_onResumeButtonPressedCallback;
	std::function<void()> m_onDefenderSelectedCallback;
};