#pragma once

#include "Animation.h"
#include "GUI/Widgets.h"
#include "defender/DefenderTypes.h"
#include "utilities/CallbackRegistry.h"

#include <functional>
#include <optional>
#include <vector>

class GUI;
class Screen;
class Atlas;
struct UIText;

struct HUDDefenderData {
	DefenderType type;
	Animation animation;
	int cost;
	float cooldown{};
	float maxCooldown{};
	bool canAfford{true};
	bool isHover{false};
};

struct ProgressBarData {
	float value{};
	float max{};
	Vector2 position{};
	Color bkgColor{};
	Color fillColor{};
};

struct HUDData {
	float batteryCharge{};
	float maxBatteryCharge{};
	int scrapsAmount{};
	int numberOfEnemiesDefeated{};
	int numberOfEnemiesToDefeat{};
	std::vector<HUDDefenderData> defenders;
	std::vector<ProgressBarData> progressBars;
	std::string levelName;

	std::optional<int> selectedDefenderIndex;

	struct OccupiedCell {
		int row{};
		int column{};
	};

	std::vector<OccupiedCell> occupiedCells;
};

class HUD {
public:
	HUD(GUI& gui);

	void update(float dt);
	void setVisible(bool visible);
	void setEnable(bool enabled);

	// Events
	CallbackHandle onDefenderSelected(std::function<void(int)> callback);

	// Data
	auto& data() {
		return m_data;
	}

	void clear();

private:
	void drawBatteryCharge(Atlas& atlas, const Rectangle& bounds);
	Vector2 measureDefenders(const Vector2& availableSize);
	void drawDefenders(Atlas& atlas, const Rectangle& bounds);
	void drawProgressBars(Atlas& atlas, const Rectangle& bounds);
	void drawProgressBar(float value, float max, const Vector2& pos, Color bkgColor = DARKGRAY, Color fillColor = GREEN);

	bool m_isEnabled{true};
	HUDData m_data;
	GUI& m_gui;

	CallbackRegistry<const int&> m_onDefenderSelectedCallbacks;
	Screen* m_screen{};
	WidgetHandle m_scrapTextHandle{};
	WidgetHandle m_batteryTextHandle{};
	WidgetHandle m_levelNameHandle{};
};
