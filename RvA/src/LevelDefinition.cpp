#include "LevelDefinition.h"

#include "GameRegistry.h"
#include "atlas/Atlas.h"
#include "constants.h"
#include "levels/LevelData.h"

using namespace std::string_literals;

auto lastColumn = FixedValue{COLS};
auto firstRow = FixedValue{0};
auto lastRow = FixedValue{ROWS - 1};

class KeyframeBuilder {
public:
	static KeyframeBuilder start() {
		return KeyframeBuilder{};
	}

	std::vector<Keyframe> build() {
		return std::move(m_keyframes);
	}

	KeyframeBuilder& t(float elapsedTime, KeyframeOperation operation) {
		m_internalTime += elapsedTime;
		m_keyframes.emplace_back(m_internalTime, std::move(operation));
		return *this;
	}

private:
	float m_internalTime{};
	std::vector<Keyframe> m_keyframes;
};

void LevelDefinition::registerLevels(GameRegistry& gameRegistry, Atlas& atlas) {
	registerTutorialLevels(gameRegistry, atlas);
	registerRaylibDemoVideoLevels(gameRegistry, atlas);
	registerMenuLevels(gameRegistry, atlas);

	registerBaseLevels(gameRegistry, atlas);
	registerSwitchOnOffLevel(gameRegistry, atlas);
	registerTankLevel(gameRegistry, atlas);
	registerSwitchOnOffLevel2(gameRegistry, atlas);
	registerPortalLevel(gameRegistry, atlas);
}

void LevelDefinition::registerTutorialLevels(GameRegistry& gameRegistry, Atlas& atlas) {
	auto sprite = [&atlas](const char* spriteName) { return atlas.getSpriteInfo(spriteName); };

	auto solarPanel = "SolarpanelTut1"s;
	auto shooter = "ShooterTut1"s;
	auto alien1 = "DefaultAlienTut1"s;
	auto alien2 = "DefaultAlienTut2"s;
	auto alien3 = "DefaultAlienTut3"s;
	auto alien4 = "DefaultAlienTut4"s;
	auto alien5 = "DefaultAlienTut5"s;

	gameRegistry.addLevel(
		"tutorial",
		{
			.name = "Tutorial",
			.musicId = "level2",
			.startingScraps = 20,
			.maxBatteryCharge = 100,
			.winCountdownDuration = 2.f,
			.winCondition = AllWavesGoneCondition{},
			.loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
			.groundBackground = sprite("ground_bkg"),
			.topBackground = sprite("top_bkg"),
			.isProgressionLevel = false,
			.timeline = {.keyframes =
							 KeyframeBuilder::start()
								 .t(0.f, HUDOperation{.type = HUDOperationType::Disable})
								 .t(0.f, HUDOperation{.type = HUDOperationType::HideDefenderPicker})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::HidePlate})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowResources})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowSkipButton})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::HideDefenderOverlay})
								 .t(0.f, MessageOperation{.text = "Let's go over the basics.", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})
								 .t(2.f, SpawnEntityOperation{.row = FixedValue{3}, .column = lastColumn, .id = FixedValue{alien1}})
								 .t(1.65f, MessageOperation{.text = "Here's an enemy Alien!", .textPosition = Vector2{50, 125}, .textHAlign = HAlign::Right, .timer = 2.0f})
								 .t(2.35f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{2}, .id = FixedValue{shooter}, .type = EntityType::Defender})
								 .t(0.0f, MessageOperation{.text = "This is one of your bots", .textPosition = Vector2{80, 125}, .timer = 3.0f})
								 .t(3.0f, MessageOperation{.text = "They attack aliens automatically!", .textPosition = Vector2{60, 125}, .timer = 3.0f})
								 .t(3.0f, MessageOperation{.text = "but they use energy to operate.", .textPosition = Vector2{60, 125}, .timer = 3.0f})
								 .t(3.0f, MessageOperation{.text = "If the battery runs out... it's GAME OVER!", .textPosition = Vector2{64, 70}, .timer = 3.f})
								 .t(3.0f, MessageOperation{.text = "Better disabling the Shooter bot!", .textPosition = Vector2{64, 70}, .timer = 3.0f})
								 .t(2.0f, EnableDefenderOperation{.enable = false, .targetIndex = FixedValue{0}, .targetId = FixedValue{shooter}})
								 .t(1.0f,
									MessageOperation{
										.text = "Build a solar panel.",
										.textPosition = Vector2{0, 250},
										.textHAlign = HAlign::Center,
										.timer = 3.f,
									})

								 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowDefenderPicker})
								 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = solarPanel})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::Enable})
								 .t(0.0f, UpdateValidCellOperation{.cells = {{FixedValue{3}, FixedValue{0}}, {FixedValue{3}, FixedValue{1}}}})

								 .t(0.0f, CheckOperation{.check = [solarPanel](const LevelData& levelData) { return levelData.getEnableDefenderAmount(solarPanel) == 1; }})
								 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::Reset, .id = solarPanel})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::HideDefenderPicker})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::Disable})

								 .t(0.0f, MessageOperation{.text = "Nice!", .textPosition = Vector2{64, 70}, .timer = 1.5f})
								 .t(1.5f, MessageOperation{.text = "Solar Panel generates energy.", .textPosition = Vector2{64, 70}, .timer = 3.0f})
								 .t(3.f, MessageOperation{.text = "Now we can start the bot again.", .textPosition = Vector2{64, 70}, .timer = 3.0f})
								 .t(0.0f, MessageOperation{.text = "Left click on the bot.", .textPosition = Vector2{90, 125}, .timer = 3.0f})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowDefenderOverlay})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::Enable})
								 .t(0.0f, CheckOperation{.check = [shooter](const LevelData& levelData) { return levelData.getEnableDefenderAmount(shooter) == 1; }})
								 .t(0.f, MessageOperation{.text = "You can always decide to turn them off again.", .textPosition = Vector2{64, 70}, .timer = 6.0f})

								 .t(0.f, SpawnEntityOperation{.row = FixedValue{3}, .column = lastColumn, .id = FixedValue{alien2}})
								 .t(0.0f, SpawnEntityBurstOperation{.amount = FixedValue{8}, .interval = FixedValue{2.f}, .row = FixedValue{3}, .column = lastColumn, .id = FixedValue{alien2}})
								 .t(3.5f,
									MessageOperation{.text = "Aliens drop scraps when they die.",
													 .highlightPosition = {392, 158},
													 .highlightSize = {32, 32},
													 .textPosition = Vector2{40, 105},
													 .textHAlign = HAlign::Right,
													 .timer = 4.f})
								 .t(4.5f,
									MessageOperation{.text = "You use scraps to build bots!",
													 .highlightPosition = {392, 158},
													 .highlightSize = {32, 32},
													 .textPosition = Vector2{40, 105},
													 .textHAlign = HAlign::Right,
													 .timer = 4.f})

								 .t(6.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = lastColumn, .id = FixedValue{alien3}})
								 .t(1.0f, MessageOperation{.text = "Oh no!", .textPosition = Vector2{64, 70}, .timer = 1.5f})
								 .t(2.0f, MessageOperation{.text = "This lane has no defences.", .textPosition = Vector2{64, 70}, .timer = 2.5f})
								 .t(3.5f, MessageOperation{.text = "Aliens will try to destroy the battery.", .textPosition = Vector2{64, 110}, .timer = 3.0f})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = lastColumn, .id = FixedValue{alien3}})
								 .t(3.5f, MessageOperation{.text = "The battery electrocutes them.", .textPosition = Vector2{64, 110}, .timer = 3.0f})
								 .t(3.5f, MessageOperation{.text = "But it consumes a lot of charge.", .textPosition = Vector2{64, 110}, .timer = 3.0f})

								 .t(0.0f, UpdateValidCellOperation{.cells = {{FixedValue{0}, FixedValue{0}}, {FixedValue{0}, FixedValue{1}}, {FixedValue{0}, FixedValue{2}}}})
								 .t(3.0f,
									MessageOperation{
										.text = "Place a Shooter in the first row.",
										.textPosition = Vector2{0, 250},
										.textHAlign = HAlign::Center,
										.timer = 6.f,
									})

								 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowDefenderPicker})
								 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = shooter})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::Enable})

								 .t(0.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.numberOfEnabledDefenders == 3; }})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::HideDefenderPicker})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::Disable})

								 .t(1.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = lastColumn, .id = FixedValue{alien4}})
								 .t(1.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = lastColumn, .id = FixedValue{alien4}})
								 .t(0.5f, MessageOperation{.text = "This lane is now defended.", .textPosition = Vector2{64, 110}, .timer = 3.0f})
								 .t(5.0f,
									MessageOperation{
										.text = "You should reinforce the other lanes too.",
										.textPosition = Vector2{0, 250},
										.textHAlign = HAlign::Center,
										.timer = 5.f,
									})

								 .t(0.0f, UpdateValidCellOperation{.clearAll = true})
								 .t(5.0f,
									MessageOperation{
										.text = "Add 4 more bots to prepare to the big wave.",
										.textPosition = Vector2{0, 250},
										.textHAlign = HAlign::Center,
										.timer = 5.f,
									})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowDefenderPicker})
								 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::Reset})
								 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = shooter})
								 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = solarPanel})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::Enable})
								 .t(5.0f, HUDOperation{.type = HUDOperationType::ShowPlate})
								 .t(0.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.numberOfEnabledDefenders == 7; }})
								 .t(0.0f,
									MessageOperation{
										.text = "Defeat all the aliens to win this level.",
										.textHAlign = HAlign::Center,
										.textVAlign = VAlign::Center,
										.timer = 6.f,
									})
								 .t(6.0f,
									MessageOperation{
										.text = "3...",
										.textHAlign = HAlign::Center,
										.textVAlign = VAlign::Center,
										.timer = 1.f,
									})
								 .t(1.0f,
									MessageOperation{
										.text = "2...",
										.textHAlign = HAlign::Center,
										.textVAlign = VAlign::Center,
										.timer = 1.f,
									})
								 .t(1.0f,
									MessageOperation{
										.text = "1...",
										.textHAlign = HAlign::Center,
										.textVAlign = VAlign::Center,
										.timer = 1.f,
									})
								 .t(1.0f,
									SpawnEntityBurstOperation{.amount = FixedValue{5},
															  .interval = RandomRange{0.5f, 1.f},
															  .row = Selection<int>{{0, 1}, {3, 1}},
															  .column = lastColumn,
															  .id = FixedValue{alien5}})
								 .t(0.0f,
									MessageOperation{
										.text = "THEY'RE COMING!!!",
										.textHAlign = HAlign::Center,
										.textVAlign = VAlign::Center,
										.timer = 3.f,
									})
								 .t(10.0f,
									SpawnEntityBurstOperation{.amount = FixedValue{10}, .interval = RandomRange{0.5f, 1.f}, .row = RandomRange{0, 6}, .column = lastColumn, .id = FixedValue{alien5}})
								 .t(0.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})
								 .build()},
		});
}

void LevelDefinition::registerMenuLevels(GameRegistry& gameRegistry, Atlas& atlas) {
	auto sprite = [&atlas](const char* spriteName) { return atlas.getSpriteInfo(spriteName); };
	std::string b1 = "B1";
	std::string b2 = "B2";
	std::string portal = "Portal";

	auto selection = EntitySelection{{b1, 0.7f}, {b2, 0.2f}, {portal, 0.1f}};
	auto defenderSelection = EntitySelection{{"Solarpanel", 0.25f}, {"Lasertron", 0.25f}, {"Catapult", 0.25f}, {"Shooter", 0.25f}};
	gameRegistry.addLevel("menuLevel",
						  {.name = "",
						   .musicId = "level1",
						   .startingScraps = 900,
						   .maxBatteryCharge = 1000,
						   .winCountdownDuration = 2.f,
						   .winCondition = AllWavesGoneCondition{},
						   .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
						   .groundBackground = sprite("ground_bkg"),
						   .topBackground = sprite("top_bkg"),
						   .isProgressionLevel = false,
						   .timeline = {.keyframes = {
											{0.f, SpawnEntityOperation{.row = RandomRange{0, 6}, .column = FixedValue{0}, .id = defenderSelection, .type = EntityType::Defender}},
											{0.f, SpawnEntityOperation{.row = RandomRange{0, 6}, .column = FixedValue{1}, .id = defenderSelection, .type = EntityType::Defender}},
											{0.f, SpawnEntityOperation{.row = RandomRange{0, 6}, .column = FixedValue{2}, .id = defenderSelection, .type = EntityType::Defender}},
											{0.f, SpawnEntityOperation{.row = RandomRange{0, 6}, .column = FixedValue{3}, .id = defenderSelection, .type = EntityType::Defender}},
											{0,
											 SpawnEntityBurstOperation{
												 .amount = FixedValue{2000},
												 .interval = RandomRange{1.f, 4.f},
												 .row = RandomRange{0, 6},
												 .column = lastColumn,
												 .id = selection,
											 }},
										}}});
}

void LevelDefinition::registerRaylibDemoVideoLevels(GameRegistry& gameRegistry, Atlas& atlas) {
	auto sprite = [&atlas](const char* spriteName) { return atlas.getSpriteInfo(spriteName); };

	gameRegistry.addDefenderFromTemplate("Solarpanel", "SP1", [&](DefenderTypeInfo& info) { info.batteryDrain = -20; });

	gameRegistry.addLevel(
		"raylibDemo",
		{
			.name = "",
			.musicId = "level2",
			.startingScraps = 200,
			.maxBatteryCharge = 100,
			.winCountdownDuration = 2.f,
			.winCondition = AllWavesGoneCondition{},
			.loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
			.groundBackground = sprite("ground_bkg"),
			.topBackground = sprite("top_bkg"),
			.isProgressionLevel = false,
			.timeline = {.keyframes =
							 KeyframeBuilder::start()
								 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = "Shooter"})
								 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = "SP1"})
								 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = "Catapult"})
								 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = "Lasertron"})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::Enable})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowDefenderOverlay})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{1}, .column = FixedValue{2}, .id = FixedValue{"Shooter"s}, .type = EntityType::Defender})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{2}, .column = FixedValue{4}, .id = FixedValue{"Shooter"s}, .type = EntityType::Defender, .enabled = false})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{4}, .id = FixedValue{"Shooter"s}, .type = EntityType::Defender, .enabled = false})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{4}, .column = FixedValue{4}, .id = FixedValue{"Shooter"s}, .type = EntityType::Defender, .enabled = false})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{0}, .id = FixedValue{"SP1"s}, .type = EntityType::Defender, .enabled = true})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{4}, .column = FixedValue{0}, .id = FixedValue{"SP1"s}, .type = EntityType::Defender, .enabled = true})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{5}, .column = FixedValue{0}, .id = FixedValue{"SP1"s}, .type = EntityType::Defender, .enabled = true})
								 .t(1.0f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(0, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{2.f, 4.f}, .row = FixedValue{3}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(2.0f,
									MessageOperation{
										.text = "THEY'RE COMING!!!",
										.fontSize = FONT_MEDIUM,
										.textHAlign = HAlign::Center,
										.textVAlign = VAlign::Center,
										.timer = 2.f,
									})
								 .t(0.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{2}, .column = lastColumn, .id = FixedValue{"B2"s}})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(1.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(0.5f, SpawnEntityOperation{.row = FixedValue{4}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(1.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = lastColumn, .id = FixedValue{"B1"s}})

								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{6}, .column = lastColumn, .id = FixedValue{"PortalDemo"s}})

								 .t(1.0f, SpawnEntityOperation{.row = FixedValue{4}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(1.5f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{3}, .column = lastColumn, .id = FixedValue{"B2"s}})

								 .t(8.0f, SpawnEntityOperation{.row = FixedValue{2}, .column = lastColumn, .id = FixedValue{"B2"s}})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(1.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(0.5f, SpawnEntityOperation{.row = FixedValue{4}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(1.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = lastColumn, .id = FixedValue{"B1"s}})

								 .t(1.0f, SpawnEntityOperation{.row = FixedValue{4}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(1.5f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{"B1"s}})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{3}, .column = lastColumn, .id = FixedValue{"B2"s}})

								 .build()},
		});
}

void LevelDefinition::registerBaseLevels(GameRegistry& gameRegistry, Atlas& atlas) {
	// This level forces the players to work with enable/disable state
	// The bots are already placed in the map and there's no scrap gain.
	// The solar panels are limited and the player has to enable the correct defenders to face the waves

	auto sprite = [&atlas](const char* spriteName) { return atlas.getSpriteInfo(spriteName); };

	auto shooter = "Shooter"s;
	auto catapult = "Catapult"s;
	auto solar = "Solarpanel"s;
	auto lasertron = "Lasertron"s;

	auto alien1 = "B1"s;

	gameRegistry.addLevel(
		"level1",
		{.name = "An easy one",
		 .musicId = "level1",
		 .startingScraps = 150,
		 .maxBatteryCharge = 150,
		 .winCountdownDuration = 2.f,
		 .winCondition = AllWavesGoneCondition{},
		 .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
		 .groundBackground = sprite("ground_bkg"),
		 .topBackground = sprite("top_bkg"),
		 .timeline = {
			 .keyframes =
				 KeyframeBuilder::start()
					 .t(0.f, HUDOperation{.type = HUDOperationType::Enable})
					 .t(0.f, HUDOperation{.type = HUDOperationType::ShowDefenderPicker})
					 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowPlate})
					 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowResources})
					 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowDefenderOverlay})
					 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = solar})
					 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = shooter})

					 .t(2.0f, MessageOperation{.text = "They're coming!!!", .fontSize = FONT_MEDIUM, .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

					 .t(2.f, SpawnEntityOperation{.row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(0.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})
					 .t(2.f, SpawnEntityBurstOperation{.amount = FixedValue{10}, .interval = RandomRange{3.0f, 5.f}, .row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien1}})

					 .t(30.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.enemyCount == 0; }})

					 .t(1.5f, MessageOperation{.text = "I give you some time", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

					 .t(3.5f, SpawnEntityOperation{.row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(0.0f, SpawnEntityBurstOperation{.amount = FixedValue{14}, .interval = RandomRange{3.0f, 5.f}, .row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(0.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})

					 .t(0.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.enemyCount == 0; }})

					 .build()}});

	gameRegistry.addLevel(
		"level2",
		{.name = "Let's raise the bar",
		 .musicId = "level1",
		 .startingScraps = 250,
		 .maxBatteryCharge = 150,
		 .winCountdownDuration = 2.f,
		 .winCondition = AllWavesGoneCondition{},
		 .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
		 .groundBackground = sprite("ground_bkg"),
		 .topBackground = sprite("top_bkg"),
		 .timeline = {.keyframes =
						  KeyframeBuilder::start()
							  .t(0.f, HUDOperation{.type = HUDOperationType::Enable})
							  .t(0.f, HUDOperation{.type = HUDOperationType::ShowDefenderPicker})
							  .t(0.0f, HUDOperation{.type = HUDOperationType::ShowPlate})
							  .t(0.0f, HUDOperation{.type = HUDOperationType::ShowResources})
							  .t(0.0f, HUDOperation{.type = HUDOperationType::ShowDefenderOverlay})
							  .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = solar})
							  .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = shooter})
							  .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = lasertron})

							  .t(2.0f, MessageOperation{.text = "They're coming!!!", .fontSize = FONT_MEDIUM, .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

							  .t(2.f, SpawnEntityOperation{.row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien1}})
							  .t(0.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})
							  .t(2.f, SpawnEntityBurstOperation{.amount = FixedValue{20}, .interval = RandomRange{3.0f, 5.f}, .row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien1}})

							  .t(30.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.enemyCount == 0; }})

							  .t(1.5f, MessageOperation{.text = "Nice, but this was just the first wave...", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 4.0f})
							  .t(4.0f, MessageOperation{.text = "Attaaaaaack!!!!", .fontSize = FONT_MEDIUM, .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

							  .t(2.5f, SpawnEntityBurstOperation{.amount = FixedValue{5}, .interval = RandomRange{0.5f, 1.f}, .row = FixedValue(0), .column = lastColumn, .id = FixedValue{alien1}})
							  .t(0.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})
							  .t(5.0f, SpawnEntityBurstOperation{.amount = FixedValue{5}, .interval = RandomRange{0.5f, 1.f}, .row = FixedValue(2), .column = lastColumn, .id = FixedValue{alien1}})
							  .t(5.0f, SpawnEntityBurstOperation{.amount = FixedValue{5}, .interval = RandomRange{0.5f, 1.f}, .row = FixedValue(4), .column = lastColumn, .id = FixedValue{alien1}})
							  .t(5.0f, SpawnEntityBurstOperation{.amount = FixedValue{5}, .interval = RandomRange{0.5f, 1.f}, .row = FixedValue(6), .column = lastColumn, .id = FixedValue{alien1}})
							  .t(5.0f, SpawnEntityBurstOperation{.amount = FixedValue{5}, .interval = RandomRange{0.5f, 1.f}, .row = FixedValue(5), .column = lastColumn, .id = FixedValue{alien1}})
							  .t(5.0f, SpawnEntityBurstOperation{.amount = FixedValue{5}, .interval = RandomRange{0.5f, 1.f}, .row = FixedValue(3), .column = lastColumn, .id = FixedValue{alien1}})
							  .t(5.0f, SpawnEntityBurstOperation{.amount = FixedValue{5}, .interval = RandomRange{0.5f, 1.f}, .row = FixedValue(1), .column = lastColumn, .id = FixedValue{alien1}})

							  .build()}});
}

void LevelDefinition::registerSwitchOnOffLevel(GameRegistry& gameRegistry, Atlas& atlas) {
	// This level forces the players to work with enable/disable state
	// The bots are already placed in the map and there's no scrap gain.
	// The solar panels are limited and the player has to enable the correct defenders to face the waves

	auto sprite = [&atlas](const char* spriteName) { return atlas.getSpriteInfo(spriteName); };

	auto shooter = "Shooter"s;
	auto catapult = "Catapult"s;
	auto solar = "Solarpanel"s;
	auto lasertron = "Lasertron"s;

	auto alien1 = "noScrapAlien1"s;
	gameRegistry.addEnemyFromTemplate("B1", alien1, [&](EnemyTypeInfo& info) { info.dropType = std::nullopt; });

	gameRegistry.addLevel(
		"switchOnOff",
		{.name = "On & Off & On & Off",
		 .musicId = "level2",
		 .startingScraps = 0,
		 .maxBatteryCharge = 150,
		 .winCountdownDuration = 2.f,
		 .winCondition = AllWavesGoneCondition{},
		 .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
		 .groundBackground = sprite("ground_bkg"),
		 .topBackground = sprite("top_bkg"),
		 .skipActions = {{.id = "SkipIntro", .operations = {}}},
		 .timeline = {.keyframes = KeyframeBuilder::start()
									   .t(0.f, HUDOperation{.type = HUDOperationType::Enable})
									   .t(0.f, HUDOperation{.type = HUDOperationType::HideDefenderPicker})
									   .t(0.0f, HUDOperation{.type = HUDOperationType::HidePlate})
									   .t(0.0f, HUDOperation{.type = HUDOperationType::ShowResources})
									   .t(0.0f, HUDOperation{.type = HUDOperationType::ShowDefenderOverlay})

									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{1}, .column = FixedValue{2}, .id = FixedValue{shooter}, .type = EntityType::Defender, .enabled = false})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{2}, .column = FixedValue{2}, .id = FixedValue{shooter}, .type = EntityType::Defender, .enabled = false})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{4}, .column = FixedValue{2}, .id = FixedValue{shooter}, .type = EntityType::Defender, .enabled = false})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{5}, .column = FixedValue{2}, .id = FixedValue{shooter}, .type = EntityType::Defender, .enabled = false})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{0}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = true})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{1}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = true})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{2}, .column = FixedValue{0}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = true})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{4}, .column = FixedValue{0}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = true})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = FixedValue{0}, .id = FixedValue{lasertron}, .type = EntityType::Defender, .enabled = false})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{6}, .column = FixedValue{0}, .id = FixedValue{lasertron}, .type = EntityType::Defender, .enabled = false})

									   .t(0.2f, HUDOperation{.type = HUDOperationType::ShowSkipButton})
									   .t(0.0f, MessageOperation{.text = "Aliens here do not drop scraps!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 5.0f})
									   .t(5.0f, MessageOperation{.text = "Gonna win with what we already have!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 6.0f})
									   .t(6.0f, MessageOperation{.text = "Enable or disable just what you need!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 6.0f})
									   .t(6.0f, MessageOperation{.text = "3...", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 1.0f})
									   .t(1.0f, MessageOperation{.text = "2...", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 1.0f})
									   .t(1.0f, MessageOperation{.text = "1...", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 1.0f})
									   .t(1.0f, SkipTagOperation{.id = "SkipIntro"})
									   .t(0.0f, HUDOperation{.type = HUDOperationType::HideSkipButton})
									   .t(0.0f, MessageOperation{.text = "They're coming!!!", .fontSize = FONT_MEDIUM, .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

									   .t(0.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{2}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{4}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{5}, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(2.f, SpawnEntityOperation{.row = FixedValue{4}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{2}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{5}, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(2.f, SpawnEntityOperation{.row = FixedValue{5}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{4}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{2}, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(0.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.enemyCount == 0; }})

									   .t(0.5f, MessageOperation{.text = "Great!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 1.5f})
									   .t(1.5f, MessageOperation{.text = "Too easy, right?", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})
									   .t(3.0f, MessageOperation{.text = "Let's try again!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

									   .t(2.5f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = firstRow, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(0.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})

									   .t(6.f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = lastRow, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(5.f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = firstRow, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(4.f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = lastRow, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(0.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.enemyCount == 0; }})

									   .t(0.5f, MessageOperation{.text = "What???!??!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 1.5f})
									   .t(1.5f, MessageOperation{.text = "Ok, you know how to use Lasertrons then...", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 5.0f})
									   .t(5.0f, MessageOperation{.text = "But can you use all your bots at once???", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 5.0f})
									   .t(5.0f, MessageOperation{.text = "Let's gooo!!!", .fontSize = FONT_MEDIUM, .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

									   .t(0.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})

									   .t(2.f, SpawnEntityOperation{.row = FixedValue{5}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{2}, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(2.f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = firstRow, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(2.f, SpawnEntityOperation{.row = FixedValue{4}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{5}, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(2.f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = lastRow, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(2.f, SpawnEntityOperation{.row = FixedValue{2}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{alien1}})

									   .build()}});
}

void LevelDefinition::registerTankLevel(GameRegistry& gameRegistry, Atlas& atlas) {
	// Presentation of the Tank-Speedy alien

	auto sprite = [&atlas](const char* spriteName) { return atlas.getSpriteInfo(spriteName); };

	auto shooter = "Shooter"s;
	auto catapult = "Catapult"s;
	auto solar = "Solarpanel"s;
	auto lasertron = "Lasertron"s;

	auto alien1 = "B1"s;
	auto alien2 = "B2"s;

	gameRegistry.addLevel(
		"tank",
		{.name = "So... they have Berserkers too?",
		 .musicId = "level1",
		 .startingScraps = 100,
		 .maxBatteryCharge = 100,
		 .winCountdownDuration = 2.f,
		 .winCondition = AllWavesGoneCondition{},
		 .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
		 .groundBackground = sprite("ground_bkg"),
		 .topBackground = sprite("top_bkg"),
		 .skipActions = {{.id = "SkipIntro",
						  .operations = {ClearEntityOperation{.row = FixedValue{3}, .column = FixedValue{4}, .type = EntityType::Defender}, ClearAllEntityOperation{.type = EntityType::Enemy}}}},
		 .timeline = {
			 .keyframes =
				 KeyframeBuilder::start()
					 .t(0.f, HUDOperation{.type = HUDOperationType::Disable})
					 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowSkipButton})

					 .t(0.0f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{4}, .id = FixedValue{lasertron}, .type = EntityType::Defender, .enabled = true})
					 .t(0.0f, SpawnEntityOperation{.row = FixedValue{2}, .column = FixedValue{0}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = true})
					 .t(0.0f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{0}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = true})
					 .t(0.0f, SpawnEntityOperation{.row = FixedValue{4}, .column = FixedValue{0}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = true})
					 .t(0.f, SpawnEntityOperation{.row = FixedValue(3), .column = lastColumn, .id = FixedValue{alien2}})

					 .t(12.0f, MessageOperation{.text = "Wtf was that????", .fontSize = FONT_SMALL, .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})
					 .t(3.0f, MessageOperation{.text = "It's way stronger than the others! Damn...", .fontSize = FONT_SMALL, .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

					 .t(3.0f, SkipTagOperation{.id = "SkipIntro"})
					 .t(0.0f, HUDOperation{.type = HUDOperationType::HideSkipButton})
					 .t(0.f, HUDOperation{.type = HUDOperationType::ShowDefenderPicker})
					 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowPlate})
					 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowResources})
					 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowDefenderOverlay})
					 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = solar})
					 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = shooter})
					 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = lasertron})
					 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = catapult})
					 .t(0.f, HUDOperation{.type = HUDOperationType::Enable})

					 .t(2.f, SpawnEntityOperation{.row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien2}})
					 .t(10.f, SpawnEntityOperation{.row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien2}})
					 .t(5.f, SpawnEntityOperation{.row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien2}})
					 .t(5.f, SpawnEntityOperation{.row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien2}})

					 .t(10.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.enemyCount == 0; }})

					 .t(2.0f, MessageOperation{.text = "They're coming!!!", .fontSize = FONT_MEDIUM, .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})
					 .t(0.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})

					 .t(0.0f, SpawnEntityBurstOperation{.amount = FixedValue{15}, .interval = RandomRange{3.0f, 5.f}, .row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(1.0f, SpawnEntityBurstOperation{.amount = FixedValue{5}, .interval = FixedValue{8.0f}, .row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien2}})

					 .t(0.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.enemyCount == 0; }})

					 .build()}});
}

void LevelDefinition::registerSwitchOnOffLevel2(GameRegistry& gameRegistry, Atlas& atlas) {
	// This level forces the players to work with enable/disable state
	// The bots are already placed in the map and there's no scrap gain.
	// The solar panels are limited and the player has to enable the correct defenders to face the waves

	auto sprite = [&atlas](const char* spriteName) { return atlas.getSpriteInfo(spriteName); };

	auto shooter = "Shooter"s;
	auto catapult = "Catapult"s;
	auto solar = "Solarpanel"s;
	auto lasertron = "Lasertron"s;

	auto alien1 = "noScrapAlien1"s;
	gameRegistry.addEnemyFromTemplate("B1", alien1, [&](EnemyTypeInfo& info) { info.dropType = std::nullopt; });

	gameRegistry.addLevel(
		"switchOnOff2",
		{.name = "On & Off & On & Off 2",
		 .musicId = "level2",
		 .startingScraps = 0,
		 .maxBatteryCharge = 100,
		 .winCountdownDuration = 2.f,
		 .winCondition = AllWavesGoneCondition{},
		 .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
		 .groundBackground = sprite("ground_bkg"),
		 .topBackground = sprite("top_bkg"),
		 .skipActions = {{.id = "SkipIntro", .operations = {}}},
		 .timeline = {.keyframes = KeyframeBuilder::start()
									   .t(0.f, HUDOperation{.type = HUDOperationType::Enable})
									   .t(0.f, HUDOperation{.type = HUDOperationType::HideDefenderPicker})
									   .t(0.0f, HUDOperation{.type = HUDOperationType::HidePlate})
									   .t(0.0f, HUDOperation{.type = HUDOperationType::ShowResources})
									   .t(0.0f, HUDOperation{.type = HUDOperationType::ShowDefenderOverlay})
									   .t(0.0f, HUDOperation{.type = HUDOperationType::ShowSkipButton})

									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = FixedValue{5}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = true})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{2}, .column = FixedValue{5}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = true})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{4}, .column = FixedValue{5}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = true})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{6}, .column = FixedValue{5}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = true})

									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{0}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = true})

									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = FixedValue{0}, .id = FixedValue{catapult}, .type = EntityType::Defender, .enabled = false})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{6}, .column = FixedValue{0}, .id = FixedValue{catapult}, .type = EntityType::Defender, .enabled = false})

									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = FixedValue{1}, .id = FixedValue{lasertron}, .type = EntityType::Defender, .enabled = false})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{6}, .column = FixedValue{1}, .id = FixedValue{lasertron}, .type = EntityType::Defender, .enabled = false})

									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = FixedValue{4}, .id = FixedValue{shooter}, .type = EntityType::Defender, .enabled = false})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{2}, .column = FixedValue{4}, .id = FixedValue{shooter}, .type = EntityType::Defender, .enabled = false})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{4}, .column = FixedValue{4}, .id = FixedValue{shooter}, .type = EntityType::Defender, .enabled = false})
									   .t(0.0f, SpawnEntityOperation{.row = FixedValue{6}, .column = FixedValue{4}, .id = FixedValue{shooter}, .type = EntityType::Defender, .enabled = false})

									   .t(0.2f, MessageOperation{.text = "Aliens here do not drop scraps!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 5.0f})
									   .t(5.0f, MessageOperation{.text = "3...", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 1.0f})
									   .t(1.0f, MessageOperation{.text = "2...", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 1.0f})
									   .t(1.0f, MessageOperation{.text = "1...", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 1.0f})

									   .t(1.0f, SkipTagOperation{.id = "SkipIntro"})
									   .t(0.0f, HUDOperation{.type = HUDOperationType::HideSkipButton})
									   .t(0.0f, MessageOperation{.text = "They're coming!!!", .fontSize = FONT_MEDIUM, .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

									   .t(0.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{2}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{4}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{5}, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(2.f, SpawnEntityOperation{.row = FixedValue{4}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{2}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{5}, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(2.f, SpawnEntityOperation{.row = FixedValue{5}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{4}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{2}, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(0.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.enemyCount == 0; }})

									   .t(0.5f, MessageOperation{.text = "Great!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 1.5f})
									   .t(1.5f, MessageOperation{.text = "That was probably too easy tho...", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})
									   .t(3.0f, MessageOperation{.text = "Let's try again!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

									   .t(2.5f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = firstRow, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(0.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})

									   .t(6.f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = lastRow, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(5.f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = firstRow, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(4.f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = lastRow, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(0.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.enemyCount == 0; }})

									   .t(0.5f, MessageOperation{.text = "It's not over yet.", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 5.0f})
									   .t(5.0f, MessageOperation{.text = "They're coming!!!", .fontSize = FONT_MEDIUM, .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

									   .t(0.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})

									   .t(2.f, SpawnEntityOperation{.row = FixedValue{5}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{2}, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(2.f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = firstRow, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(2.f, SpawnEntityOperation{.row = FixedValue{4}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{5}, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(2.f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = lastRow, .column = lastColumn, .id = FixedValue{alien1}})

									   .t(2.f, SpawnEntityOperation{.row = FixedValue{2}, .column = lastColumn, .id = FixedValue{alien1}})
									   .t(2.f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{alien1}})

									   .build()}});
}

void LevelDefinition::registerPortalLevel(GameRegistry& gameRegistry, Atlas& atlas) {
	// Presentation of the portal alien

	auto sprite = [&atlas](const char* spriteName) { return atlas.getSpriteInfo(spriteName); };

	auto shooter = "Shooter"s;
	auto catapult = "Catapult"s;
	auto solar = "Solarpanel"s;
	auto lasertron = "Lasertron"s;

	auto alien1 = "B1"s;
	auto alien2 = "B2"s;
	auto portal = "Portal"s;
	auto alienPortalFirst = "alienPortalFirst"s;

	gameRegistry.addEnemyFromTemplate("Portal", alienPortalFirst, [&](EnemyTypeInfo& info) {
		info.behavior = PortalSpawnBehaviorInfo{
			.timeBeforeActing = 3.0f,
			.timeBeforeActingAgain = 1.f,
			.chanceIncreaseOverTime = 1.f,
			.baseChanceToSpawn = 100.f,
			.columnDistance = FixedValue{8},
			.rowDistance = FixedValue{-1},
			.portalCastRange = FixedValue{2},
			.animation = {sprite("portal_alien_summon"), 0.1f, 1},
		};
	});

	gameRegistry.addLevel(
		"portal",
		{.name = "Tele...   ...port",
		 .musicId = "level2",
		 .startingScraps = 200,
		 .maxBatteryCharge = 100,
		 .winCountdownDuration = 2.f,
		 .winCondition = AllWavesGoneCondition{},
		 .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
		 .groundBackground = sprite("ground_bkg"),
		 .topBackground = sprite("top_bkg"),
		 .skipActions = {{.id = "SkipIntro",
						  .operations = {ClearEntityOperation{.row = FixedValue{2}, .column = FixedValue{0}, .type = EntityType::Defender},
										 ClearAllEntityOperation{.type = EntityType::Portal},
										 ClearAllEntityOperation{.type = EntityType::Enemy},
										 ClearAllEntityOperation{.type = EntityType::Bullet}}}},
		 .timeline = {
			 .keyframes =
				 KeyframeBuilder::start()
					 .t(0.f, HUDOperation{.type = HUDOperationType::Disable})
					 .t(0.f, HUDOperation{.type = HUDOperationType::ShowSkipButton})

					 .t(0.0f, SpawnEntityOperation{.row = FixedValue{2}, .column = FixedValue{0}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = true})
					 .t(0.0f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{0}, .id = FixedValue{shooter}, .type = EntityType::Defender, .enabled = true})
					 .t(0.0f, SpawnEntityOperation{.row = FixedValue{4}, .column = FixedValue{0}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = true})
					 .t(0.f, SpawnEntityOperation{.row = FixedValue(3), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(1.0f, SpawnEntityOperation{.row = FixedValue(3), .column = lastColumn, .id = FixedValue{alienPortalFirst}})
					 .t(3.f, SpawnEntityOperation{.row = FixedValue(3), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(0.5f, SpawnEntityOperation{.row = FixedValue(3), .column = lastColumn, .id = FixedValue{alien1}})

					 .t(8.0f, MessageOperation{.text = "They can teleport????", .fontSize = FONT_SMALL, .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 4.0f})

					 .t(4.0f, SkipTagOperation{.id = "SkipIntro"})
					 .t(0.0f, HUDOperation{.type = HUDOperationType::HideSkipButton})
					 .t(0.0f, MessageOperation{.text = "They're coming!!!", .fontSize = FONT_MEDIUM, .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

					 .t(0.f, HUDOperation{.type = HUDOperationType::ShowDefenderPicker})
					 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowPlate})
					 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowResources})
					 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowDefenderOverlay})
					 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = solar})
					 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = shooter})
					 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = lasertron})
					 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = catapult})
					 .t(0.f, HUDOperation{.type = HUDOperationType::Enable})

					 .t(1.f, SpawnEntityOperation{.row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(5.f, SpawnEntityOperation{.row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(0.f, FlagTimelineOperation{.icon = "icon_alien_timeline"})
					 .t(5.f, SpawnEntityOperation{.row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien2}})
					 .t(5.f, SpawnEntityOperation{.row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(5.f, SpawnEntityOperation{.row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien1}})

					 .t(0.f, SpawnEntityOperation{.row = FixedValue(2), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(1.0f, SpawnEntityOperation{.row = FixedValue(2), .column = lastColumn, .id = FixedValue{portal}})
					 .t(3.f, SpawnEntityOperation{.row = FixedValue(2), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(0.5f, SpawnEntityOperation{.row = FixedValue(2), .column = lastColumn, .id = FixedValue{alien1}})

					 .t(10.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.enemyCount == 0; }})

					 .t(2.0f, MessageOperation{.text = "Final Wave!!!", .fontSize = FONT_MEDIUM, .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

					 .t(0.0f, SpawnEntityBurstOperation{.amount = FixedValue{10}, .interval = RandomRange{3.0f, 5.f}, .row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(0.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})

					 .t(1.0f, SpawnEntityBurstOperation{.amount = FixedValue{5}, .interval = FixedValue{6.0f}, .row = RandomRange(0, 6), .column = lastColumn, .id = FixedValue{alien2}})

					 .t(0.f, SpawnEntityOperation{.row = FixedValue(4), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(1.0f, SpawnEntityOperation{.row = FixedValue(4), .column = lastColumn, .id = FixedValue{portal}})
					 .t(3.f, SpawnEntityOperation{.row = FixedValue(4), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(0.5f, SpawnEntityOperation{.row = FixedValue(4), .column = lastColumn, .id = FixedValue{alien1}})

					 .t(0.f, SpawnEntityOperation{.row = FixedValue(1), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(1.0f, SpawnEntityOperation{.row = FixedValue(1), .column = lastColumn, .id = FixedValue{portal}})
					 .t(3.f, SpawnEntityOperation{.row = FixedValue(1), .column = lastColumn, .id = FixedValue{alien1}})
					 .t(0.5f, SpawnEntityOperation{.row = FixedValue(1), .column = lastColumn, .id = FixedValue{alien1}})

					 .build()}});
}
