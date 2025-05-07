#pragma once

#include <variant>

struct RandomRange {
	float min{0};
	float max{0};
};

struct FixedValue {
	float value{0};
};

using ConfigValue = std::variant<RandomRange, FixedValue>;

float getConfigValue(const ConfigValue& value);
float getConfigValue(const FixedValue& fixedValue);
float getConfigValue(const RandomRange& randomRange);
