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

class Collider
{
	// object that owns the collider
	// we don't use a reference because we want to be able to copy the collider
	MYLIB_OO_ENCAPSULATE_PTR(Object*, object)

	// displacement from object's position
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(Vector, ds)
	
	// half size of the collider
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(Vector, half_size)

	// id of the collider
	MYLIB_OO_ENCAPSULATE_SCALAR(uint32_t, id)

public:
	// We want to force the user to set the object
	Collider () = delete;

	Collider (Object *object_, const Vector& ds_, const Vector& size_, const uint32_t id_)
		: object(object_), ds(ds_), half_size(size_ / fp(2)), id(id_)
	{
	}

	/*
		Function that checks if the collider is colliding with another collider.
		Returns a pair in which:
		- The first element is true if the colliders are colliding, false otherwise.
		- The second element is the displacement vector that the object "b" should move to stop colliding.
	*/

	static std::pair<bool, Vector> check_collision (const Collider& a, const Collider& b);


#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__
	void render (const Color& color) const;
#endif
};

// ---------------------------------------------------

} // end namespace Game

#endif