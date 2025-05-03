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
	bool drawPause{};
	std::vector<HUDDefenderData> defenders;
	std::vector<ProgressBarData> progressBars;

	std::optional<DefenderType> selectedDefender;
};

class HUD
{
public:
	HUD(Atlas& atlas, GUI& gui);

	void draw();
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
	void drawPause();
	void drawProgressBars();
	void drawProgressBar(float value, float max, const Vector2& pos, Color bkgColor = DARKGRAY, Color fillColor = GREEN);


	bool m_defenderHover{};
	HUDData m_data;
	Atlas& m_atlas;
	GUI& m_gui;

	std::function<void()> m_onPauseButtonPressedCallback;
	std::function<void()> m_onMenuButtonPressedCallback;
	std::function<void()> m_onResumeButtonPressedCallback;
	std::function<void()> m_onDefenderSelectedCallback;
};