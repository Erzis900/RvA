#include "LevelDefinition.h"

#include "GameRegistry.h"
#include "atlas/Atlas.h"
#include "constants.h"

using namespace std::string_literals;

void LevelDefinition::registerLevels(GameRegistry& gameRegistry, Atlas& atlas) {
	auto sprite = [&atlas](const char* spriteName) { return atlas.getSpriteInfo(spriteName); };

	constexpr int MAX_BATTERY_CHARGE = 100;
	std::string b1 = "B1";
	std::string b2 = "B2";
	std::string portal = "Portal";

	auto lastColumn = FixedValue{18};

	gameRegistry.addLevel(
		"tutorial1",
		{
			.name = "Tutorial 1",
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
							{0.f, HUDOperation{.type = HUDOperationType::Disable}},
							{0.5f, TutorialOperation{.text = "Welcome to Game Name. This is a tutorial."}},
							{2.f, SpawnEntityOperation{.row = FixedValue{3}, .column = lastColumn, .id = FixedValue{"B1"s}}},
							{3.65f, TutorialOperation{.text = "This is an Alien. Aliens are bad!", .highlightPosition = {GAME_RENDERTEXTURE_SIZE.x - 50, 170}, .highlightSize = {32, 32}}},
							{5.f, HUDOperation{.type = HUDOperationType::Enable}},
						},
				},
		});

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
