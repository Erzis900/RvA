#pragma once

#include <variant>

template<typename T> struct LessThan {
	T value{};
};

template<typename T> struct GreaterThan {
	T value{};
};

template<typename T> struct LessThanOrEqual {
	T value{};
};

template<typename T> struct GreaterThanOrEqual {
	T value{};
};

template<typename T> struct EqualTo {
	T value{};
};

template<typename T> struct NotEqualTo {
	T value{};
};

template<typename T> struct CheckRangeIn {
	T min{};
	T max{};
};

template<typename T> struct CheckRangeOut {
	T min{};
	T max{};
};

template<typename T> struct CheckValueCondition {
	T value{};
};

template<typename T>
using ConfigConditionBase = std::variant<LessThan<T>, GreaterThan<T>, LessThanOrEqual<T>, GreaterThanOrEqual<T>, EqualTo<T>, NotEqualTo<T>, CheckRangeIn<T>, CheckRangeOut<T>, CheckValueCondition<T>>;

template<typename T> class ConfigCondition : public ConfigConditionBase<T> {
public:
	using ConfigConditionBase<T>::ConfigConditionBase;

	bool check(T value) const {
		return std::visit([&](auto&& arg) { return checkCondition(arg, value); }, *this);
	}
};

template<typename T> bool checkCondition(const LessThan<T>& condition, const T& value) {
	return value < condition.value;
}

template<typename T> bool checkCondition(const GreaterThan<T>& condition, const T& value) {
	return value > condition.value;
}

template<typename T> bool checkCondition(const LessThanOrEqual<T>& condition, const T& value) {
	return value <= condition.value;
}

template<typename T> bool checkCondition(const GreaterThanOrEqual<T>& condition, const T& value) {
	return value >= condition.value;
}

template<typename T> bool checkCondition(const EqualTo<T>& condition, const T& value) {
	return value == condition.value;
}

template<typename T> bool checkCondition(const NotEqualTo<T>& condition, const T& value) {
	return value != condition.value;
}

template<typename T> bool checkCondition(const CheckRangeIn<T>& condition, const T& value) {
	return value >= condition.min && value <= condition.max;
}

template<typename T> bool checkCondition(const CheckRangeOut<T>& condition, const T& value) {
	return value < condition.min || value > condition.max;
}
