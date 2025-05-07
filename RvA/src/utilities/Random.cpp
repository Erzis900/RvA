#include "Random.h"

Random* Random::_instance = nullptr;

Random::Random() {
	std::random_device device;
	generator = std::mt19937(device());
}

void Random::setInstance(Random* instance) {
	_instance = instance;
}

float Random::range(float min, float max) {
	return min + (max - min) * random();
}

int Random::range(int min, int max, int step) {
	int start = (min + step - 1) / step;
	int end = max / step;
	auto distribution = std::uniform_int_distribution<int>(start, end);
	int value = distribution(_instance->generator);
	return value * step;
}

int Random::range(int min, int max) {
	auto distribution = std::uniform_int_distribution<int>(min, max);
	return distribution(_instance->generator);
}

float Random::random() {
	using limits = std::numeric_limits<float>;
	auto distribution = std::uniform_real_distribution<float>(0, 1);
	return distribution(_instance->generator);
}

Vector2 Random::random2D() {
	return Vector2(random(), random());
}

Vector2 Random::range2D(float min, float max) {
	return Vector2(range(min, max), range(min, max));
}

Vector2 Random::range2D(const Vector2& min, const Vector2& max) {
	return Vector2(range(min.x, max.x), range(min.y, max.y));
}

Vector2 Random::random2D(const Vector2& min, const Vector2& max) {
	return Vector2(range(min.x, max.x), range(min.y, max.y));
}

Vector2 Random::randomPointOnCircle(float radius) {
	auto angle = Random::random() * 2 * PI;
	radius *= std::sqrt(Random::random()); // uniform distribution in a circle
	return {radius * std::cos(angle), radius * std::sin(angle)};
}
