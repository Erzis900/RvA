#pragma once

#include "utilities/Random.h"

#include <variant>

template<class... Ts> struct overloaded : Ts... {
	using Ts::operator()...;
};

template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

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

template<typename T> struct WeightedSelection {
	T value{};
	float weight{1};
};

template<typename T> struct Selection : public std::vector<WeightedSelection<T>> {
	using std::vector<WeightedSelection<T>>::vector;
};

template<typename T> class ConfigValue : public std::variant<RandomRange<T>, RandomRangeStep<T>, FixedValue<T>, Selection<T>> {
public:
	T generate() const {
		return getConfigValue(*this);
	}
};

template<typename T> T getConfigValue(const ConfigValue<T>& value) {
	return std::visit([](auto&& arg) { return getConfigValue(arg); }, value);
}

template<typename T> T getConfigValue(const FixedValue<T>& fixedValue) {
	return fixedValue.value;
}

template<typename T> T getConfigValue(const RandomRange<T>& randomRange) {
	if constexpr (std::is_arithmetic_v<T>) {
		return Random::range(randomRange.min, randomRange.max);
	}
	return {};
}

template<typename T> T getConfigValue(const RandomRangeStep<T>& randomRangeStep) {
	if constexpr (std::is_arithmetic_v<T>) {
		return Random::range(randomRangeStep.min, randomRangeStep.max, randomRangeStep.step);
	}
	return {};
}

template<typename T> T getConfigValue(const Selection<T>& selection) {
	float totalWeight = 0.0f;
	// We could avoid to calculate the total weight everytime if we expect the sum to always be 1. ( Risky tho )
	for (const auto& item : selection) {
		totalWeight += item.weight;
	}

	float threshold = Random::random() * totalWeight;
	float cumulative = 0.0f;
	for (const auto& item : selection) {
		cumulative += item.weight;
		if (threshold <= cumulative) {
			return item.value;
		}
	}
	return selection.back().value;
}
