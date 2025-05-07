#include "ConfigValue.h"

float getConfigValue(const ConfigValue& value) {
	return std::visit([&](auto&& arg) { return getConfigValue(arg); }, value);
}

float getConfigValue(const FixedValue& fixedValue) {
	return fixedValue.value;
}

float getConfigValue(const RandomRange& randomRange) {
	return randomRange.min + (GetRandomValue(0, 100) / 100.f) * (randomRange.max - randomRange.min);
}
