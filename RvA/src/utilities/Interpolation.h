#pragma once

#include <functional>
#include <raylib.h>

Color colorLerp(Color from, Color to, float time);

template<typename T> struct InterpolationBase {
	InterpolationBase<T>& start(T from, T to, float duration) {
		time = 0;
		this->duration = duration;
		this->from = from;
		this->to = to;
		started = true;
		return *this;
	}

	const auto& getValue() const {
		return value;
	}

	InterpolationBase<T>& onTick(std::function<void(const T&)> onTick) {
		onTickCallback = std::move(onTick);
		return *this;
	}

	InterpolationBase<T>& onComplete(std::function<void()> onComplete) {
		onCompleteCallback = std::move(onComplete);
		return *this;
	}

	float duration{};
	float time{};
	T value{};
	T from{};
	T to{};
	bool started{};
	std::function<void(const T&)> onTickCallback{};
	std::function<void()> onCompleteCallback{};
};

template<typename T = float> class Interpolation : public InterpolationBase<T> {
public:
	void update(float dt) {
		if (this->started) {
			this->time += dt;
			float t = this->time / this->duration;
			this->value = this->from + (t * (this->to - this->from));
			if (this->onTickCallback) {
				this->onTickCallback(this->value);
			}
			if (this->time >= this->duration) {
				this->started = false;
				this->onCompleteCallback();
			}
		}
	}
};

template<> class Interpolation<Color> : public InterpolationBase<Color> {
public:
	void update(float dt);
};
