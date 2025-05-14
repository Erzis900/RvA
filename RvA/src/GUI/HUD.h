#pragma once

#include "Animation.h"
#include "GUI/FadeScreen.h"
#include "GUI/Widgets.h"
#include "GameAction.h"
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
	bool tutorialEnabled{};
	float tutorialTime{};
	TutorialAction tutorialAction{};
	bool showResources{};
	bool showDefenderPicker{};

	std::optional<int> selectedDefenderIndex;

	struct OccupiedCell {
		int row{};
		int column{};
	};

	std::vector<OccupiedCell> occupiedCells;
};

class HUD {
public:
	HUD(GUI& gui, ResourceSystem& resourceSystem);

	void update(float dt);
	void setVisible(bool visible);
	void setEnable(bool enabled);

	// Events
	CallbackHandle onDefenderSelected(std::function<void(int)> callback);
	CallbackHandle onTutorialNext(std::function<void()> callback);

	// Data
	auto& data() {
		return m_data;
	}

	void clear();

	void startFadeInOut(std::function<void()> onFadingInDone, std::function<void()> onFadingOutDone, float seconds);

private:
	Vector2 measureDefenders(const Vector2& availableSize);
	void drawDefenders(Atlas& atlas, const Rectangle& bounds);
	void drawProgressBars(Atlas& atlas, const Rectangle& bounds);
	void drawProgressBar(float value, float max, const Vector2& pos, Color bkgColor = DARKGRAY, Color fillColor = GREEN);
	void drawTutorial(Atlas& atlas);

	bool m_isEnabled{true};
	HUDData m_data;
	GUI& m_gui;
	ResourceSystem& m_resourceSystem;

	CallbackRegistry<const int&> m_onDefenderSelectedCallbacks;
	CallbackRegistry<> m_onTutorialNextCallbacks;
	Screen* m_screen{};
	WidgetHandle m_scrapTextHandle{};
	WidgetHandle m_batteryTextHandle{};
	WidgetHandle m_batteryFillHandle{};
	WidgetHandle m_batteryIndicatorHandle{};
	WidgetHandle m_levelNameHandle{};
	WidgetHandle m_plateContainerHandle{};
	WidgetHandle m_plateTextHandle{};
	WidgetHandle m_batteryAndScrapsHandle{};
	WidgetHandle m_defenderPickerHandle{};
	FadeScreen m_fadeScreen;
	bool m_isAnyDefenderHovered{};
	int m_hoveredDefenderIndex{};
};
