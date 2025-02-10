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

#include <aurora/types.h>
#include <aurora/object.h>


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

public:
	// We want to force the user to set the object
	Collider () = delete;

	Collider (Object *object_, const Vector& ds_, const Vector& size_)
		: object(object_), ds(ds_), half_size(size_ / fp(2))
	{
	}

	/*
		Function that checks if the collider is colliding with another collider.
		Returns a pair in which:
		- The first element is true if the colliders are colliding, false otherwise.
		- The second element is the displacement vector that the object "b" should move to stop colliding.
	*/

	static std::pair<bool, Vector> check_collision (const Collider& a, const Collider& b);

	inline Vector get_world_pos () const noexcept
	{
		return this->object->get_ref_pos() + this->ds;
	}
};

// ---------------------------------------------------

} // end namespace Game

#endif