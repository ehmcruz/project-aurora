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


namespace Game
{

// ---------------------------------------------------

using MyGlib::dprint;
using MyGlib::dprintln;

constexpr ClockDuration float_to_ClockDuration (const float t)
{
	return std::chrono::duration_cast<ClockDuration>(std::chrono::duration<float>(t));
}

constexpr float ClockDuration_to_float (const ClockDuration& d)
{
	return std::chrono::duration_cast<std::chrono::duration<float>>(d).count();
}

// ---------------------------------------------------

} // end namespace Game

#endif