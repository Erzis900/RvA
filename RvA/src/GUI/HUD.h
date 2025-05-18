#pragma once

#include "Animation.h"
#include "GUI/FadeScreen.h"
#include "GUI/Widgets.h"
#include "GameAction.h"
#include "bullet/BulletTypes.h"
#include "defender/DefenderTypes.h"
#include "utilities/CallbackRegistry.h"

#include <functional>
#include <optional>
#include <vector>

class GUI;
class Screen;
class ResourceSystem;
class Atlas;
struct UIText;

struct HUDDefenderData {
	std::string name;
	std::string id;
	Animation animation;
	int cost;
	float batteryDrain{};
	DamageInfo damage{};
	float cooldown{};
	float maxCooldown{};
	bool canAfford{true};
	bool isHover{false};
};

struct HUDDeployedDefenderData {
	DefenderState state{};
	Vector2 position;
};

struct HUDTimelineDataItem {
	float time;
	std::string icon;
};

struct HUDTimelineData {
	std::vector<HUDTimelineDataItem> waves;
	float time{};
	float duration{};
};

struct ProgressBarData {
	float value{};
	float max{};
	Vector2 position{};
};

struct HUDData {
	float batteryCharge{};
	float prevBatteryCharge{};
	float batteryChargeDiff{};
	float maxBatteryCharge{};
	int scrapsAmount{};
	int numberOfEnemiesDefeated{};
	int numberOfEnemiesToDefeat{};
	std::vector<HUDDefenderData> pickableDefenders;
	std::vector<ProgressBarData> progressBars;
	std::vector<HUDDeployedDefenderData> deployedDefenders;
	HUDTimelineData timelineData;
	std::string levelName;
	bool showMessage{};
	float messageTime{};
	MessageAction messageAction{};
	bool showResources{};
	bool showTimeline{};
	bool showDefenderPicker{};
	bool showDefenderOverlay{};
	bool showPlate{};

	std::optional<int> selectedDefenderIndex;

	struct OccupiedCell {
		int row{};
		int column{};
	};

	std::function<bool(int, int)> isValidBuildCellCallback;
};

class HUD {
public:
	HUD(GUI& gui, ResourceSystem& resourceSystem);

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

	void startFadeInOut(std::function<void()> onFadingInDone, std::function<void()> onFadingOutDone, float seconds);

private:
	Vector2 measureDefenders(const Vector2& availableSize);
	Vector2 measureBatteryTrend(const Vector2& availableSize);
	void drawDefenders(Atlas& atlas, const Rectangle& bounds);
	void drawProgressBars(Atlas& atlas, const Rectangle& bounds);
	void drawProgressBar(float value, float max, const Vector2& pos, Color bkgColor = DARKGRAY, Color fillColor = GREEN);
	void drawMessage(Atlas& atlas);
	void drawDeployedDefenderHUD(Atlas& atlas, const Rectangle& bounds);
	void drawTimeline(Atlas& atlas, const Rectangle& bounds);
	void drawBatteryTrend(Atlas& atlas, const Rectangle& bounds);

	bool m_isEnabled{true};
	HUDData m_data;
	GUI& m_gui;
	ResourceSystem& m_resourceSystem;

	CallbackRegistry<const int&> m_onDefenderSelectedCallbacks;
	Screen* m_screen{};
	WidgetHandle m_scrapTextHandle{};
	WidgetHandle m_batteryTextHandle{};
	WidgetHandle m_batteryTipHandle{};
	WidgetHandle m_batteryFillHandle{};
	WidgetHandle m_batteryBottomHandle{};
	WidgetHandle m_batteryIndicatorHandle{};
	WidgetHandle m_batteryStatIconHandle{};
	WidgetHandle m_levelNameHandle{};
	WidgetHandle m_plateContainerHandle{};
	WidgetHandle m_plateTextHandle{};
	WidgetHandle m_plateDescriptionHandle{};
	WidgetHandle m_batteryAndScrapsHandle{};
	WidgetHandle m_defenderPickerHandle{};
	FadeScreen m_fadeScreen;
	bool m_isAnyDefenderHovered{};
	int m_hoveredDefenderIndex{};
};
