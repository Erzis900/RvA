#pragma once

class GameRegistry;
class Atlas;

class LevelDefinition {
public:
	static void registerLevels(GameRegistry& gameRegistry, Atlas& atlas);

private:
	static void registerTutorialLevels(GameRegistry& gameRegistry, Atlas& atlas);
};
