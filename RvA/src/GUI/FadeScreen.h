#pragma once

#include "utilities/Interpolation.h"

class FadeScreen {
public:
	void update(float dt);
	void draw();

	bool isFading() const;

	void startFadingInOut(std::function<void()> onFadingInDone, std::function<void()> onFadingOutDone, float seconds);
	void startFadingIn(std::function<void()> onFadingInDone, float seconds);
	void startFadingOut(std::function<void()> onFadingOutDone, float seconds);

private:
	Interpolation<Color> m_fading;
};
