#ifndef __PROJECT_AURORA_GAMEOBJECT_HEADER_H__
#define __PROJECT_AURORA_GAMEOBJECT_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <string>

#include <my-lib/std.h>
#include <my-lib/macros.h>

#include <my-game-lib/my-game-lib.h>

#include <aurora/types.h>


namespace Game
{

// ---------------------------------------------------

class World;

// ---------------------------------------------------

class Object
{
protected:
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(Vector, pos)
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(Vector, vel)
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(std::string, name)
	MYLIB_OO_ENCAPSULATE_PTR(World*, world)

public:
	inline Object (World *world_)
		: world(world_)
	{
	}

	virtual ~Object () = default;

	virtual void physics (const float dt, const Uint8 *keys);
	virtual void render (const float dt) = 0;
};

// ---------------------------------------------------




// ---------------------------------------------------

} // end namespace Game

#endif