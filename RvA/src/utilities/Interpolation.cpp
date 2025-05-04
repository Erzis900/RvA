#include "utilities/Interpolation.h"

#include <raymath.h>

Color colorLerp(Color a, Color b, float t) {
    Color result;
    result.r = (unsigned char)(a.r + (b.r - a.r) * t);
    result.g = (unsigned char)(a.g + (b.g - a.g) * t);
    result.b = (unsigned char)(a.b + (b.b - a.b) * t);
    result.a = (unsigned char)(a.a + (b.a - a.a) * t);
    return result;
}

void Interpolation<Color>::update(float dt)
{
	if (started)
	{
		time += dt;
		float t = Clamp(time / duration, 0, 1);
		value = colorLerp(from, to, t);
		if(onTickCallback)
		{
			onTickCallback(value);
		}
		if (time >= duration)
		{
			started = false;
			onCompleteCallback();
		}
	}
}