#ifndef __PROJECT_AURORA_OBJECT_HEADER_H__
#define __PROJECT_AURORA_OBJECT_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <string>

#include <my-lib/std.h>
#include <my-lib/macros.h>

#include <my-game-lib/my-game-lib.h>

#include <aurora/types.h>
#include <aurora/graphics.h>


namespace Game
{

// ---------------------------------------------------

class World { };

// ---------------------------------------------------

class Object
{
protected:
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(Vector, pos)
	MYLIB_OO_ENCAPSULATE_OBJ_INIT_WITH_COPY_MOVE(Vector, vel, Vector::zero())
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(std::string, name)
	MYLIB_OO_ENCAPSULATE_PTR(World*, world)

public:
	inline Object (World *world_)
		: world(world_)
	{
	}

	virtual ~Object () = default;

	virtual void physics (const float dt);
	virtual void render (const float dt) = 0;
};

// ---------------------------------------------------

class ObjectSprite : public Object
{
private:
	MYLIB_OO_ENCAPSULATE_OBJ(Sprite, sprite)

public:
	inline ObjectSprite (World *world_, const Rect2D& rect_sprite, const TextureDescriptor& texture)
		: Object(world_), sprite(this, rect_sprite, texture)
	{
	}

	void render (const float dt) override;
};

// ---------------------------------------------------

class ObjectSpriteAnimation : public Object
{
private:
	MYLIB_OO_ENCAPSULATE_OBJ(SpriteAnimation, sprite_animation)

public:
	inline ObjectSpriteAnimation (World *world_, const Rect2D& rect_sprite, std::span<TextureDescriptor> textures, const float frame_duration)
		: Object(world_), sprite_animation(this, rect_sprite, textures, frame_duration)
	{
	}

	void render (const float dt) override;
};

// ---------------------------------------------------

} // end namespace Game

#endif