#include "FadeScreen.h"

#include "constants.h"

void FadeScreen::update(float dt) {
	m_fading.update(dt);
}

void FadeScreen::draw() {
	DrawRectangle(0, 0, static_cast<int>(UI_RENDERTEXTURE_SIZE.x), static_cast<int>(UI_RENDERTEXTURE_SIZE.y), m_fading.getValue());
}

bool FadeScreen::isFading() const {
	return m_fading.started;
}

void FadeScreen::startFadingInOut(std::function<void()> onFadingInDone, std::function<void()> onFadingOutDone, float seconds) {
	seconds *= 0.5f;
	m_fading.start(Fade(BLACK, 0.f), Fade(BLACK, 1.f), seconds).onComplete([this, fadingInDone = std::move(onFadingInDone), fadingOutDone = std::move(onFadingOutDone), seconds] {
		fadingInDone();
		m_fading.start(Fade(BLACK, 1.f), Fade(BLACK, 0.f), seconds).onComplete([this, fadingOutDone = std::move(fadingOutDone)] { fadingOutDone(); });
	});
}

void FadeScreen::startFadingIn(std::function<void()> onFadingInDone, float seconds) {
	m_fading.start(Fade(BLACK, 0.f), Fade(BLACK, 1.f), seconds).onComplete([this, fadingInDone = std::move(onFadingInDone)] { fadingInDone(); });
}

void FadeScreen::startFadingOut(std::function<void()> onFadingOutDone, float seconds) {
	m_fading.start(Fade(BLACK, 1.f), Fade(BLACK, 0.f), seconds).onComplete([this, fadingOutDone = std::move(onFadingOutDone)] { fadingOutDone(); });
}
