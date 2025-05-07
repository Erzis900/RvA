#pragma once

#include <random>
#include <raymath.h>

class Random {
public:
	Random();
	static void setInstance(Random* instance);
	static float range(float min, float max);
	static int range(int min, int max, int step);
	static int range(int min, int max);
	static float random();
	static Vector2 random2D();
	static Vector2 range2D(float min, float max);
	static Vector2 range2D(const Vector2& min, const Vector2& max);
	static Vector2 random2D(const Vector2& min, const Vector2& max);
	static Vector2 randomPointOnCircle(float radius);

private:
	std::mt19937 generator;
	static Random* _instance;
};
