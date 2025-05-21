#pragma once

class GameRegistry;
class Atlas;

class LevelDefinition {
public:
	static void registerLevels(GameRegistry& gameRegistry, Atlas& atlas);

private:
	static void registerTutorialLevels(GameRegistry& gameRegistry, Atlas& atlas);
	static void registerMenuLevels(GameRegistry& gameRegistry, Atlas& atlas);
	static void registerRaylibDemoVideoLevels(GameRegistry& gameRegistry, Atlas& atlas);

	static void registerBaseLevels(GameRegistry& gameRegistry, Atlas& atlas);
	static void registerSwitchOnOffLevel(GameRegistry& gameRegistry, Atlas& atlas);
	static void registerTankLevel(GameRegistry& gameRegistry, Atlas& atlas);
	static void registerSwitchOnOffLevel2(GameRegistry& gameRegistry, Atlas& atlas);
	static void registerPortalLevel(GameRegistry& gameRegistry, Atlas& atlas);
};
