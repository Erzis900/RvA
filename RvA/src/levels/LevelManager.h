#pragma once

#include "utilities/ConfigValue.h"

struct SpawnEnemy {
	ConfigValue<int> amount{};
	int row{};
	int column{};
	std::vector<std::string> types;
};

struct SpawnEnemyOverTime {
	ConfigValue<int> amount{};
	std::vector<std::string> types;
	ConfigValue<float> interval{};
	ConfigValue<float> duration{};
};

struct Keyframe {
	float time;
	std::variant<SpawnEnemy, SpawnEnemyOverTime> action;
};

struct Timeline {
	std::vector<Keyframe> keyframes;
};

/*
void foo() {
	Timeline timeline;
	timeline.keyframes.push_back({10.f, SpawnEnemy{.amount = FixedValue{5}, .types = {"enemy1", "enemy2"}}});

	// clang-format off
	Timeline::build()
		.at(10)
			.spawn({"enemy1", "enemy2"})
			.count(5)
			.column(Random{});
		.at(20)
			.spawnOverTime({"enemy1", "enemy2"})
			.count(2)
			.interval(0.5f)
			.duration(20.f)
			.column(Random{});
		.at(30)
			.spawn({"enemy1", "enemy2"})
			.count(5)
			.row(0)
	// clang-format on
}
*/
/*
 * The LevelManager class is responsible for updating the state of the current level
 * It tells when to spawn new waves, when to spawn new enemies, and when the player win or lose
 */
class LevelManager {
public:
	LevelManager() = default;
	void update(float dt);

	void startLevel();

	float getWaves() const;

	void onSpawnWaveRequest();
	void onSpawnEnemyRequest();
	void onVictoryConditionFulfilled();
	void onLosingConditionFulfilled();

private:
};
