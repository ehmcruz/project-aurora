#ifndef __PROJECT_AURORA_COLLISION_HEADER_H__
#define __PROJECT_AURORA_COLLISION_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <utility>

#include <my-lib/std.h>
#include <my-lib/macros.h>

#include <my-game-lib/my-game-lib.h>

#include <aurora/config.h>
#include <aurora/types.h>


namespace Game
{

// ---------------------------------------------------

class Object;

// ---------------------------------------------------

struct Collider
{
	// object that owns the collider
	// we don't use a reference because we want to be able to copy the collider
	StaticObject *object;

	// displacement from object's position
	Vector ds;
	
	// size of the collider
	Vector size;

	// id of the collider
	uint32_t id;

#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__
	void render (const Color& color) const;
#endif
};

// ---------------------------------------------------

/*
	Function that checks if the collider is colliding with another collider.
	Returns a pair in which:
	- The first element is true if the colliders are colliding, false otherwise.
	- The second element is the displacement vector that the object "b" should move to stop colliding.
*/

std::pair<bool, Vector> check_collision (const Collider& a, const Collider& b);

// ---------------------------------------------------

} // end namespace Game

#endif