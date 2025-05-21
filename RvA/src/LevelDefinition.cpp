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

	registerSwitchOnOffLevel(gameRegistry, atlas);
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
			.timeline = {.keyframes =
							 KeyframeBuilder::start()
								 .t(0.f, HUDOperation{.type = HUDOperationType::Disable})
								 .t(0.f, HUDOperation{.type = HUDOperationType::HideDefenderPicker})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::HidePlate})
								 .t(0.0f, HUDOperation{.type = HUDOperationType::ShowResources})
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
			.timeline = {.keyframes =
							 KeyframeBuilder::start()
								 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = "ShooterDemo"})
								 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = "SolarpanelTutorial"})
								 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = "Catapult"})
								 .t(0.0f, DefenderPickerOperation{.type = DefenderPickerOperationType::AddItem, .id = "LasertronDemo"})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{1}, .column = FixedValue{2}, .id = FixedValue{"ShooterDemo"s}, .type = EntityType::Defender})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{4}, .id = FixedValue{"ShooterDemo"s}, .type = EntityType::Defender, .enabled = false})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{0}, .id = FixedValue{"SolarpanelTutorial"s}, .type = EntityType::Defender, .enabled = false})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{4}, .column = FixedValue{0}, .id = FixedValue{"SolarpanelTutorial"s}, .type = EntityType::Defender, .enabled = false})
								 .t(0.0f, SpawnEntityOperation{.row = FixedValue{5}, .column = FixedValue{0}, .id = FixedValue{"SolarpanelTutorial"s}, .type = EntityType::Defender, .enabled = false})
								 .t(1.0f, SpawnEntityOperation{.row = FixedValue{1}, .column = FixedValue{8}, .id = FixedValue{"B1"s}})
								 .t(0,
									SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{2.f, 4.f}, .row = FixedValue{3}, .column = lastColumn, .id = FixedValue{"B1Tutorial"s}})
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

								 .build()},
		});
}

void LevelDefinition::registerSwitchOnOffLevel(GameRegistry& gameRegistry, Atlas& atlas) {
	// This level forces the players to work with enable/disable state
	// The bots are already placed in the map and there's no scrap gain.
	// The solar panels are limited and the player has to enable the correct defenders to face the waves

	auto sprite = [&atlas](const char* spriteName) { return atlas.getSpriteInfo(spriteName); };

	auto shooter = "Shooter"s;
	auto catapult = "Catapult"s;
	auto solar = "Solarpanel"s;

	auto alien1 = "noScrapAlien1"s;
	gameRegistry.addEnemyFromTemplate("B1", alien1, [&](EnemyTypeInfo& info) {
		info.maxHp = 100;
		info.speed = 50;
		info.attackTime = 0.35f;
		info.dropType = std::nullopt;
	});

	auto lasertron = "LasertronSpecial"s;
	gameRegistry.addDefenderFromTemplate("Lasertron", lasertron, [&](DefenderTypeInfo& info) {
		info.batteryDrain = 15;
		info.bulletType = "LaserBeamSpecial";
		info.firstShootCooldown = 1.0f;
		info.shootCooldown = 1.0f;
	});

	gameRegistry.addBulletFromTemplate<LaserBeamData>("LaserBeam", "LaserBeamSpecial", [&](LaserBeamData& info) { info.damage.baseDamage = 200.f; });

	gameRegistry.addLevel(
		"switchOnOff",
		{.name = "On - Off - On - Off",
		 .musicId = "level1",
		 .startingScraps = 0,
		 .maxBatteryCharge = 100,
		 .winCountdownDuration = 2.f,
		 .winCondition = AllWavesGoneCondition{},
		 .loseCondition = BatteryLevelCondition{.batteryLevel = LessThanOrEqual{0.f}},
		 .groundBackground = sprite("ground_bkg"),
		 .topBackground = sprite("top_bkg"),
		 .timeline = {.keyframes =
						  KeyframeBuilder::start()
							  .t(0.f, HUDOperation{.type = HUDOperationType::Enable})
							  .t(0.f, HUDOperation{.type = HUDOperationType::HideDefenderPicker})
							  .t(0.0f, HUDOperation{.type = HUDOperationType::HidePlate})
							  .t(0.0f, HUDOperation{.type = HUDOperationType::ShowResources})
							  .t(0.0f, HUDOperation{.type = HUDOperationType::ShowDefenderOverlay})

							  .t(0.0f, SpawnEntityOperation{.row = FixedValue{1}, .column = FixedValue{2}, .id = FixedValue{shooter}, .type = EntityType::Defender, .enabled = false})
							  .t(0.0f, SpawnEntityOperation{.row = FixedValue{2}, .column = FixedValue{2}, .id = FixedValue{shooter}, .type = EntityType::Defender, .enabled = false})
							  .t(0.0f, SpawnEntityOperation{.row = FixedValue{4}, .column = FixedValue{2}, .id = FixedValue{shooter}, .type = EntityType::Defender, .enabled = false})
							  .t(0.0f, SpawnEntityOperation{.row = FixedValue{5}, .column = FixedValue{2}, .id = FixedValue{shooter}, .type = EntityType::Defender, .enabled = false})
							  .t(0.0f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{0}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = false})
							  .t(0.0f, SpawnEntityOperation{.row = FixedValue{3}, .column = FixedValue{1}, .id = FixedValue{solar}, .type = EntityType::Defender, .enabled = false})
							  .t(0.0f, SpawnEntityOperation{.row = FixedValue{0}, .column = FixedValue{0}, .id = FixedValue{lasertron}, .type = EntityType::Defender, .enabled = false})
							  .t(0.0f, SpawnEntityOperation{.row = FixedValue{6}, .column = FixedValue{0}, .id = FixedValue{lasertron}, .type = EntityType::Defender, .enabled = false})

							  .t(0.2f, MessageOperation{.text = "Aliens here don't drop scraps!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 5.0f})
							  .t(5.0f, MessageOperation{.text = "Gonna win with what we already have in the fields!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 6.0f})
							  .t(6.0f, MessageOperation{.text = "Remember to enable or disable just what you need!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 6.0f})
							  .t(6.0f, MessageOperation{.text = "3...", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 1.0f})
							  .t(1.0f, MessageOperation{.text = "2...", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 1.0f})
							  .t(1.0f, MessageOperation{.text = "1...", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 1.0f})
							  .t(1.0f, MessageOperation{.text = "They're coming!!!", .fontSize = FONT_MEDIUM, .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

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

							  .t(0.5f, MessageOperation{.text = "Great!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 1.0f})
							  .t(1.0f, MessageOperation{.text = "That was probably too easy tho...", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 4.0f})
							  .t(4.0f, MessageOperation{.text = "Let's try again!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})

							  .t(3.0f, FlagTimelineOperation{.icon = "icon_alien_timeline"})

							  .t(2.f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = firstRow, .column = lastColumn, .id = FixedValue{alien1}})
							  .t(6.f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = lastRow, .column = lastColumn, .id = FixedValue{alien1}})
							  .t(5.f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = firstRow, .column = lastColumn, .id = FixedValue{alien1}})
							  .t(4.f, SpawnEntityBurstOperation{.amount = FixedValue{6}, .interval = RandomRange{0.5f, 1.f}, .row = lastRow, .column = lastColumn, .id = FixedValue{alien1}})

							  .t(0.0f, CheckOperation{.check = [](const LevelData& levelData) { return levelData.enemyCount == 0; }})

							  .t(0.5f, MessageOperation{.text = "What???!??!", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 3.0f})
							  .t(3.0f, MessageOperation{.text = "Ok, you know how to use LaserTron then...", .textHAlign = HAlign::Center, .textVAlign = VAlign::Center, .timer = 5.0f})
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
