#include "LevelDefinition.h"

#include "GameRegistry.h"
#include "atlas/Atlas.h"
#include "constants.h"
#include "levels/LevelData.h"

using namespace std::string_literals;

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

	auto sprite = [&atlas](const char* spriteName) { return atlas.getSpriteInfo(spriteName); };

	constexpr int MAX_BATTERY_CHARGE = 100;
	std::string b1 = "B1";
	std::string b2 = "B2";
	std::string portal = "Portal";

	auto lastColumn = FixedValue{18};

	auto selection = EntitySelection{{b1, 0.7f}, {b2, 0.2f}, {portal, 200.1f}};
	gameRegistry.addLevel("level1",
						  {
							  .name = "Level 1",
							  .startingScraps = 100,
							  .maxBatteryCharge = MAX_BATTERY_CHARGE,
							  .winCountdownDuration = 2.f,
							  .winCondition = AllWavesGoneCondition{},
							  .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
							  .groundBackground = sprite("ground_bkg"),
							  .topBackground = sprite("top_bkg"),
							  .timeline =
								  {
									  .keyframes =
										  {
											  {0.f, DefenderPickerOperation{.type = DefenderPickerOperationType::Reset}},
											  {0.f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = "Solarpanel"}},
											  {0.f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = "Shooter"}},
											  {0.f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = "Catapult"}},
											  {0.f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = "Lasertron"}},
											  {0.f,
											   SpawnEntityBurstOperation{
												   .amount = FixedValue{1},
												   .interval = FixedValue{1.f},
												   .row = RandomRange{0, 6},
												   .column = lastColumn,
												   .id = selection,
											   }},
											  {3.f, SpawnEntityOperation{.row = FixedValue{3}, .column = lastColumn, .id = FixedValue{"B1"s}}},
											  {5.f, SpawnEntityOperation{.row = FixedValue{1}, .column = lastColumn, .id = FixedValue{"B1"s}}},
											  {7.f, SpawnEntityOperation{.row = FixedValue{6}, .column = lastColumn, .id = FixedValue{"B1"s}}},
											  {15.f,
											   SpawnEntityBurstOperation{
												   .amount = FixedValue{10},
												   .interval = FixedValue{1.f},
												   .row = RandomRange{0, 6},
												   .column = lastColumn,
												   .id = selection,
											   }},
										  },
								  },
						  });

	gameRegistry.addLevel("level2",
						  {
							  .name = "Level 2",
							  .startingScraps = 100,
							  .maxBatteryCharge = MAX_BATTERY_CHARGE,
							  .winCountdownDuration = 2.f,
							  .winCondition = AllWavesGoneCondition{},
							  .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
							  .groundBackground = sprite("ground_bkg"),
							  .topBackground = sprite("top_bkg"),
							  .timeline =
								  {
									  .keyframes =
										  {
											  {2.f,
											   SpawnEntityBurstOperation{
												   .amount = FixedValue{5},
												   .interval = FixedValue{1.f},
												   .row = RandomRange{0, 6},
												   .column = lastColumn,
												   .id = FixedValue{"B1"s},
											   }},
											  {10.f,
											   SpawnEntityBurstOperation{
												   .amount = FixedValue{5},
												   .interval = FixedValue{1.f},
												   .row = RandomRange{0, 6},
												   .column = lastColumn,
												   .id = FixedValue{"B2"s},
											   }},
											  {20.f,
											   SpawnEntityBurstOperation{
												   .amount = FixedValue{5},
												   .interval = FixedValue{1.f},
												   .row = RandomRange{0, 6},
												   .column = lastColumn,
												   .id = FixedValue{"Portal"s},
											   }},
										  },
								  },
						  });

	auto enemyAt = [&](int row) { return SpawnEntityOperation{.row = FixedValue{row}, .column = lastColumn, .id = FixedValue{"B1"s}}; };

	auto space = 2.5f;

	gameRegistry.addLevel("level3",
						  {
							  .name = "Hello",
							  .startingScraps = 100,
							  .maxBatteryCharge = 10000,
							  .winCountdownDuration = 2.f,
							  .winCondition = AllWavesGoneCondition{},
							  .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
							  .groundBackground = sprite("ground_bkg"),
							  .topBackground = sprite("top_bkg"),
							  .timeline = {.keyframes =
											   {
												   // H
												   {space + 0.f, enemyAt(1)},
												   {space + 0.f, enemyAt(2)},
												   {space + 0.f, enemyAt(3)},
												   {space + 0.f, enemyAt(4)},
												   {space + 0.f, enemyAt(5)},
												   {space + 0.5f, enemyAt(3)},
												   {space + 1.f, enemyAt(3)},
												   {space + 1.5f, enemyAt(1)},
												   {space + 1.5f, enemyAt(2)},
												   {space + 1.5f, enemyAt(3)},
												   {space + 1.5f, enemyAt(4)},
												   {space + 1.5f, enemyAt(5)},

												   // E
												   {space * 2 + 0.5f, enemyAt(1)},
												   {space * 2 + 0.5f, enemyAt(2)},
												   {space * 2 + 0.5f, enemyAt(3)},
												   {space * 2 + 0.5f, enemyAt(4)},
												   {space * 2 + 0.5f, enemyAt(5)},
												   {space * 2 + 1.f, enemyAt(1)},
												   {space * 2 + 1.f, enemyAt(3)},
												   {space * 2 + 1.f, enemyAt(5)},
												   {space * 2 + 1.5f, enemyAt(1)},
												   {space * 2 + 1.5f, enemyAt(3)},
												   {space * 2 + 1.5f, enemyAt(5)},

												   // L
												   {space * 3 + 0.5f, enemyAt(1)},
												   {space * 3 + 0.5f, enemyAt(2)},
												   {space * 3 + 0.5f, enemyAt(3)},
												   {space * 3 + 0.5f, enemyAt(4)},
												   {space * 3 + 0.5f, enemyAt(5)},
												   {space * 3 + 1.f, enemyAt(5)},
												   {space * 3 + 1.5f, enemyAt(5)},

												   // L
												   {space * 4 + 0.5f, enemyAt(1)},
												   {space * 4 + 0.5f, enemyAt(2)},
												   {space * 4 + 0.5f, enemyAt(3)},
												   {space * 4 + 0.5f, enemyAt(4)},
												   {space * 4 + 0.5f, enemyAt(5)},
												   {space * 4 + 1.f, enemyAt(5)},
												   {space * 4 + 1.5f, enemyAt(5)},

												   // O
												   {space * 5 + 0.5f, enemyAt(1)},
												   {space * 5 + 0.5f, enemyAt(2)},
												   {space * 5 + 0.5f, enemyAt(3)},
												   {space * 5 + 0.5f, enemyAt(4)},
												   {space * 5 + 0.5f, enemyAt(5)},
												   {space * 5 + 1.f, enemyAt(1)},
												   {space * 5 + 1.f, enemyAt(5)},
												   {space * 5 + 1.5f, enemyAt(1)},
												   {space * 5 + 1.5f, enemyAt(2)},
												   {space * 5 + 1.5f, enemyAt(3)},
												   {space * 5 + 1.5f, enemyAt(4)},
												   {space * 5 + 1.5f, enemyAt(5)},
											   }},
						  });

	auto defenderSelection = EntitySelection{{"Solarpanel", 0.25f}, {"Lasertron", 0.25f}, {"Catapult", 0.25f}, {"Shooter", 0.25f}};
	gameRegistry.addLevel("demoLevel",
						  {.name = "",
						   .startingScraps = 900,
						   .maxBatteryCharge = 1000,
						   .winCountdownDuration = 2.f,
						   .winCondition = AllWavesGoneCondition{},
						   .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
						   .groundBackground = sprite("ground_bkg"),
						   .topBackground = sprite("top_bkg"),
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

void LevelDefinition::registerTutorialLevels(GameRegistry& gameRegistry, Atlas& atlas) {
	auto sprite = [&atlas](const char* spriteName) { return atlas.getSpriteInfo(spriteName); };
	auto lastColumn = FixedValue{18};

	gameRegistry.addLevel(
		"tutorial",
		{
			.name = "Tutorial",
			.startingScraps = 0,
			.maxBatteryCharge = 100,
			.winCountdownDuration = 2.f,
			.winCondition = AllWavesGoneCondition{},
			.loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
			.groundBackground = sprite("ground_bkg"),
			.topBackground = sprite("top_bkg"),
			.timeline =
				{.keyframes =
					 KeyframeBuilder::start()
						 .t(0.f, HUDOperation{.type = HUDOperationType::Disable})
						 .t(0.f, HUDOperation{.type = HUDOperationType::HideDefenderPicker})
						 .t(0.f, HUDOperation{.type = HUDOperationType::HideResources})
						 .t(0.f, TutorialOperation{.text = "Welcome to Game Name!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})
						 .t(3.5f, TutorialOperation{.text = "Let's go over the basics.", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})
						 .t(2.f, SpawnEntityOperation{.row = FixedValue{3}, .column = lastColumn, .id = FixedValue{"B1TutorialTank"s}})
						 .t(1.65f, TutorialOperation{.text = "Here's an enemy Alien!", .textPosition = Vector2{50, 125}, .textHAlign = HAlign::Right, .timer = 2.0f})
						 .t(2.35f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{2}, .id = FixedValue{"ShooterTutorial"s}, .type = EntityType::Defender})
						 .t(0.0f, TutorialOperation{.text = "This is one of your bots", .textPosition = Vector2{80, 125}, .timer = 3.0f})
						 .t(3.0f, TutorialOperation{.text = "They attack aliens automatically!", .textPosition = Vector2{60, 125}, .timer = 3.0f})
						 .t(3.0f, TutorialOperation{.text = "but they use energy to operate.", .textPosition = Vector2{60, 125}, .timer = 3.0f})
						 .t(3.0f, TutorialOperation{.text = "Your energy comes from a battery.", .textPosition = Vector2{64, 70}, .timer = 3.f})
						 .t(3.0f, TutorialOperation{.text = "If it runs out... it's GAME OVER!", .textPosition = Vector2{64, 70}, .timer = 3.f})
						 .t(3.0f, TutorialOperation{.text = "We need a way to produce energy!", .textPosition = Vector2{64, 70}, .timer = 3.0f})
						 .t(3.5f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{0}, .id = FixedValue{"SolarpanelTutorial"s}, .type = EntityType::Defender})
						 .t(0.0f, TutorialOperation{.text = "There we go!", .textPosition = Vector2{50, 125}, .timer = 4.0f})
						 .t(4.5f, TutorialOperation{.text = "A special bot named Solar Panel!", .textPosition = Vector2{64, 70}, .timer = 4.0f})
						 .t(0.f, SpawnEntityOperation{.row = FixedValue{3}, .column = lastColumn, .id = FixedValue{"B1TutorialTank2"s}})
						 .t(4.5f, TutorialOperation{.text = "It doesn't attack but generates energy.", .textPosition = Vector2{64, 70}, .timer = 3.0f})
						 .t(0.0f, SpawnEntityBurstOperation{.amount = FixedValue{4}, .interval = RandomRange{2.f, 3.f}, .row = FixedValue{3}, .column = lastColumn, .id = FixedValue{"B1Tutorial"s}})
						 .t(3.5f,
							TutorialOperation{.text = "When an alien dies it drop scraps.",
											  .highlightPosition = {392, 158},
											  .highlightSize = {32, 32},
											  .textPosition = Vector2{40, 105},
											  .textHAlign = HAlign::Right,
											  .timer = 4.f})
						 .t(4.5f,
							TutorialOperation{.text = "You can use scraps to build more bots!",
											  .highlightPosition = {392, 158},
											  .highlightSize = {32, 32},
											  .textPosition = Vector2{40, 105},
											  .textHAlign = HAlign::Right,
											  .timer = 4.f})
						 .t(4.5f, HUDOperation{.type = HUDOperationType::ShowResources})
						 .t(0.0f,
							TutorialOperation{
								.text = "Here You can keep track of your battery level\nand how much scrap you've collected.",
								.textPosition = Vector2{30, 250},
								.timer = 6.f,
							})
						 .t(6.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = lastColumn, .id = FixedValue{"B1TutorialFast"s}})
						 .t(1.0f, TutorialOperation{.text = "Oh no!", .textPosition = Vector2{64, 70}, .timer = 1.5f})
						 .t(2.0f, TutorialOperation{.text = "This lane has no defences.", .textPosition = Vector2{64, 70}, .timer = 2.5f})
						 .t(3.5f, TutorialOperation{.text = "Aliens will try to destroy the battery.", .textPosition = Vector2{64, 110}, .timer = 3.0f})
						 .t(0.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = lastColumn, .id = FixedValue{"B1TutorialFast"s}})
						 .t(3.5f, TutorialOperation{.text = "The battery electrocutes them.", .textPosition = Vector2{64, 110}, .timer = 3.0f})
						 .t(3.5f, TutorialOperation{.text = "Electrocution is your last defense.", .textPosition = Vector2{64, 110}, .timer = 3.0f})
						 .t(3.5f, TutorialOperation{.text = "But it consumes a lot of battery charge.", .textPosition = Vector2{64, 110}, .timer = 3.0f})

						 .t(3.5f, TutorialOperation{.text = "Let's cover this lane with one extra bot.", .textPosition = Vector2{64, 110}, .timer = 3.0f})
						 .t(3.5f, SpawnEntityOperation{.row = FixedValue{0}, .column = FixedValue{2}, .id = FixedValue{"ShooterTutorial"s}, .type = EntityType::Defender, .enabled = false})
						 .t(1.0f, TutorialOperation{.text = "This time the bot is disabled.", .textPosition = Vector2{64, 110}, .timer = 3.0f})
						 .t(3.0f, TutorialOperation{.text = "A disabled bot doesn't drain energy.", .textPosition = Vector2{64, 110}, .timer = 3.0f})
						 .t(3.0f, TutorialOperation{.text = "Right-click the bot to enable/disable them", .textPosition = Vector2{64, 110}, .timer = 3.0f})
						 .t(0.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.numberOfEnabledDefenders == 3; }})
						 .t(0.0f, TutorialOperation{.text = "Well done!", .textPosition = Vector2{64, 110}, .timer = 1.5f})

						 .t(1.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = lastColumn, .id = FixedValue{"B1Tutorial"s}})
						 .t(0.5f, TutorialOperation{.text = "This lane is now defended.", .textPosition = Vector2{64, 110}, .timer = 3.0f})
						 .t(5.0f, TutorialOperation{.text = "You should reinforce the other lanes too.", .textPosition = Vector2{64, 110}, .timer = 5.0f})
						 .t(5.0f,
							TutorialOperation{
								.text = "Choose the bots you want to build from this menu.",
								.textPosition = Vector2{0, 250},
								.textHAlign = HAlign::Center,
								.timer = 6.f,
							})
						 .t(0.5f, HUDOperation{.type = HUDOperationType::ShowDefenderPicker})
						 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = "ShooterTutorial"})
						 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = "SolarpanelTutorial"})
						 .t(0.0f, HUDOperation{.type = HUDOperationType::Enable})
						 .t(0.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.numberOfEnabledDefenders == 6; }})
						 .t(0.0f,
							TutorialOperation{
								.text = "Prepare for the real wave of aliens.",
								.textPosition = Vector2{0, 250},
								.textHAlign = HAlign::Center,
								.timer = 6.f,
							})
						 .t(6.0f,
							TutorialOperation{
								.text = "If you defeat all of them you win this level.",
								.textPosition = Vector2{0, 250},
								.textHAlign = HAlign::Center,
								.timer = 6.f,
							})
						 .t(6.0f,
							TutorialOperation{
								.text = "Good Luck!",
								.textPosition = Vector2{0, 250},
								.textHAlign = HAlign::Center,
								.timer = 3.f,
							})
						 .t(3.0f,
							TutorialOperation{
								.text = "3...",
								.textPosition = Vector2{0, 250},
								.textHAlign = HAlign::Center,
								.timer = 1.f,
							})
						 .t(1.0f,
							TutorialOperation{
								.text = "2...",
								.textPosition = Vector2{0, 250},
								.textHAlign = HAlign::Center,
								.timer = 1.f,
							})
						 .t(1.0f,
							TutorialOperation{
								.text = "1...",
								.textPosition = Vector2{0, 250},
								.textHAlign = HAlign::Center,
								.timer = 1.f,
							})
						 .t(1.0f,
							TutorialOperation{
								.text = "They're Coming!!!",
								.textPosition = Vector2{0, 250},
								.textHAlign = HAlign::Center,
								.timer = 1.f,
							})
						 .t(0, SpawnEntityBurstOperation{.amount = FixedValue{5}, .interval = RandomRange{2.f, 4.f}, .row = FixedValue{3}, .column = lastColumn, .id = FixedValue{"B1Tutorial"s}})
						 .t(5.5f,
							SpawnEntityBurstOperation{.amount = FixedValue{20}, .interval = RandomRange{2.f, 4.f}, .row = RandomRange{0, 6}, .column = lastColumn, .id = FixedValue{"B1Tutorial"s}})
						 .build()},
		});
}
