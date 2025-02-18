#ifndef __PROJECT_AURORA_GLOBALS_HEADER_H__
#define __PROJECT_AURORA_GLOBALS_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <my-lib/trigger.h>
#include <my-lib/timer.h>
#include <my-lib/interpolation.h>

#include <my-game-lib/my-game-lib.h>

#include <aurora/types.h>


namespace Game
{

// ---------------------------------------------------

inline MyGlib::Lib *game_lib = nullptr;
inline MyGlib::Event::Manager *event_manager = nullptr;
inline MyGlib::Audio::Manager *audio_manager = nullptr;
inline MyGlib::Graphics::Manager *renderer = nullptr;

// ---------------------------------------------------

inline Mylib::Trigger::Timer timer( Clock::now );
using Timer = decltype(timer);

inline Mylib::InterpolationManager<float> interpolation_manager;

// ---------------------------------------------------

inline constexpr float foo = 0;

// ---------------------------------------------------

} // end namespace Game

#endif