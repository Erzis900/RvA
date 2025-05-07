#pragma once

#include "utilities/Random.h"

#include <variant>

template<typename T> struct RandomRange {
	T min{0};
	T max{0};
};

template<typename T> struct RandomRangeStep {
	T min{0};
	T max{0};
	T step{0};
};

template<typename T> struct FixedValue {
	T value{0};
};

template<typename T> class ConfigValue : public std::variant<RandomRange<T>, RandomRangeStep<T>, FixedValue<T>> {
public:
	T generate() const {
		return getConfigValue(*this);
	}
};

template<typename T> T getConfigValue(const ConfigValue<T>& value) {
	return std::visit([&](auto&& arg) { return getConfigValue(arg); }, value);
}

template<typename T> T getConfigValue(const FixedValue<T>& fixedValue) {
	return fixedValue.value;
}

template<typename T> T getConfigValue(const RandomRange<T>& randomRange) {
	return Random::range(randomRange.min, randomRange.max);
}

template<typename T> T getConfigValue(const RandomRangeStep<T>& randomRangeStep) {
	return Random::range(randomRangeStep.min, randomRangeStep.max, randomRangeStep.step);
}
