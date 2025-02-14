#ifndef __PROJECT_AURORA_GLOBALS_HEADER_H__
#define __PROJECT_AURORA_GLOBALS_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <my-game-lib/my-game-lib.h>


namespace Game
{

// ---------------------------------------------------

inline MyGlib::Lib *game_lib = nullptr;
inline MyGlib::Event::Manager *event_manager = nullptr;
inline MyGlib::Audio::Manager *audio_manager = nullptr;
inline MyGlib::Graphics::Manager *renderer = nullptr;

// ---------------------------------------------------

inline constexpr float foo = 0;

// ---------------------------------------------------

} // end namespace Game

#endif