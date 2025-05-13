#pragma once

class GameRegistry;
class Atlas;

class LevelDefinition {
public:
	static void registerLevels(GameRegistry& gameRegistry, Atlas& atlas);
};
