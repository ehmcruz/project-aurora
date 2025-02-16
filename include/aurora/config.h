#ifndef __PROJECT_AURORA_CONFIG_HEADER_H__
#define __PROJECT_AURORA_CONFIG_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <my-lib/std.h>
#include <my-lib/macros.h>

#include <aurora/types.h>


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

inline constexpr bool render_sprite_box = true;

// ---------------------------------------------------

// isometric view
inline constexpr Vector camera_vector = Vector(1, 1, -0.75);
inline constexpr Vector camera_up = Vector(0, 0, 1);

inline constexpr Vector gravity = Vector(0, 0, -9.8);

// ---------------------------------------------------

} // end namespace Config
} // end namespace Game

#endif