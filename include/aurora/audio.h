#ifndef __PROJECT_AURORA_AUDIO_HEADER_H__
#define __PROJECT_AURORA_AUDIO_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <my-lib/std.h>

#include <my-game-lib/my-game-lib.h>

#include <aurora/types.h>


namespace Game
{

// ---------------------------------------------------

namespace Audio
{
	inline AudioDescriptor spell;
	inline AudioDescriptor enemy_death;
	inline AudioDescriptor background_music;
}

void load_audio ();

// ---------------------------------------------------

} // end namespace Game

#endif