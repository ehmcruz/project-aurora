#ifndef __PROJECT_AURORA_CONFIG_HEADER_H__
#define __PROJECT_AURORA_CONFIG_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <my-lib/std.h>
#include <my-lib/macros.h>


namespace Game
{
namespace Config
{

// ---------------------------------------------------

#define AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__

// ---------------------------------------------------

#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__
	inline constexpr bool render_colliders = true;
#else
	inline constexpr bool render_colliders = false;
#endif

// ---------------------------------------------------

} // end namespace Config
} // end namespace Game

#endif