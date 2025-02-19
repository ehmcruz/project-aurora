#ifndef __PROJECT_AURORA_LIB_HEADER_H__
#define __PROJECT_AURORA_LIB_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <chrono>

#include <SDL.h>

#include <my-lib/std.h>
#include <my-lib/macros.h>
#include <my-lib/math-vector.h>

#include <my-game-lib/my-game-lib.h>

#include <aurora/types.h>
#include <aurora/globals.h>


namespace Game
{

// ---------------------------------------------------

using MyGlib::dprint;
using MyGlib::dprintln;
using MyGlib::Graphics::fp;

constexpr ClockDuration float_to_ClockDuration (const float t)
{
	return std::chrono::duration_cast<ClockDuration>(std::chrono::duration<float>(t));
}

constexpr float ClockDuration_to_float (const ClockDuration& d)
{
	return std::chrono::duration_cast<std::chrono::duration<float>>(d).count();
}

// ---------------------------------------------------

template <typename T>
T random_vector ()
{
	T r;
	std::uniform_real_distribution<typename T::Type> d(0, 1);

	for (uint32_t i = 0; i < T::get_dim(); i++)
		r[i] = d(random_generator);

	return r;
}

// ---------------------------------------------------

inline float random_float (const float min, const float max)
{
	std::uniform_real_distribution<float> d(min, max);
	return d(random_generator);
}

// ---------------------------------------------------

} // end namespace Game

#endif