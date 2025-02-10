#ifndef __PROJECT_AURORA_OBJECT_HEADER_H__
#define __PROJECT_AURORA_OBJECT_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <string>
#include <list>

#include <my-lib/std.h>
#include <my-lib/macros.h>

#include <my-game-lib/my-game-lib.h>

#include <aurora/config.h>
#include <aurora/types.h>
#include <aurora/graphics.h>
#include <aurora/collision.h>


namespace Game
{

// ---------------------------------------------------

class World;

// ---------------------------------------------------

class AbstractObject
{
	MYLIB_OO_ENCAPSULATE_PTR(World*, world)
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(std::string, name)

public:
	inline AbstractObject (World *world_)
		: world(world_)
	{
	}

	virtual ~AbstractObject () = default;

	virtual void render (const float dt) = 0;
	virtual void update (const float dt) = 0;
};

// ---------------------------------------------------

class Object : public AbstractObject
{
	MYLIB_OO_ENCAPSULATE_OBJ_INIT_WITH_COPY_MOVE(Point, pos, Point::zero())

protected:
	std::list<Collider> colliders;

public:
	inline Object (World *world_)
		: AbstractObject(world_)
	{
	}

	inline Object (World *world_, const Point& pos_)
		: AbstractObject(world_), pos(pos_)
	{
	}

#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__
	void render_colliders (const Color& color) const;
#endif
};

// ---------------------------------------------------

class DynamicObject : public Object
{
protected:
	MYLIB_OO_ENCAPSULATE_OBJ_INIT_WITH_COPY_MOVE(Vector, vel, Vector::zero())

public:
	inline DynamicObject (World *world_)
		: Object(world_)
	{
	}

	inline DynamicObject (World *world_, const Point& pos_)
		: Object(world_, pos_)
	{
	}
	
	inline void physics (const float dt) noexcept
	{
		this->pos += this->vel * dt;
	}
};

// ---------------------------------------------------
/*
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
};*/

// ---------------------------------------------------
/*
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
};*/

// ---------------------------------------------------

class PlayerObject : public DynamicObject
{
public:
	PlayerObject (World *world_, const Point& pos_);

	void render (const float dt) override final;
	void update (const float dt) override final;
};

// ---------------------------------------------------

} // end namespace Game

#endif