#ifndef __PROJECT_AURORA_GRAPHICS_HEADER_H__
#define __PROJECT_AURORA_GRAPHICS_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <vector>
#include <span>

#include <my-lib/std.h>
#include <my-lib/macros.h>

#include <my-game-lib/my-game-lib.h>

#include <aurora/types.h>


namespace Game
{

// ---------------------------------------------------

class Object;

// ---------------------------------------------------

class Sprite
{
private:
	MYLIB_OO_ENCAPSULATE_PTR_INIT(Object*, object, nullptr)
	MYLIB_OO_ENCAPSULATE_OBJ(Rect2D, rect)
	MYLIB_OO_ENCAPSULATE_OBJ(TextureDescriptor, texture)
	MYLIB_OO_ENCAPSULATE_OBJ_INIT_WITH_COPY_MOVE(Vector, ds, Vector::zero())

public:
	Sprite (Object *object_, const Rect2D& rect_, const TextureDescriptor& texture_)
		: object(object_), rect(rect_), texture(texture_)
	{
		this->rect.set_scale_y(-1);
		this->rect.calculate_vertices();
	}

	void set_scale (const float scale) noexcept
	{
		this->rect.set_scale_x(scale);
		this->rect.set_scale_y(-scale);
		this->rect.calculate_vertices();
	}

	void render ();
};

// ---------------------------------------------------

class SpriteAnimation
{
private:
	MYLIB_OO_ENCAPSULATE_PTR_INIT(Object*, object, nullptr)
	MYLIB_OO_ENCAPSULATE_OBJ_READONLY(std::vector<Sprite>, sprites)
	MYLIB_OO_ENCAPSULATE_SCALAR(float, frame_duration)
	MYLIB_OO_ENCAPSULATE_SCALAR_INIT(uint32_t, current_frame, 0)
	MYLIB_OO_ENCAPSULATE_SCALAR_INIT(float, current_frame_time, 0)

private:
	Rect2D rect;

public:
	SpriteAnimation (Object *object_, const Rect2D& rect_, std::span<TextureDescriptor> textures, const float frame_duration_);

	void set_scale (const float scale) noexcept;
	void render (const float dt);
};

// ---------------------------------------------------

} // end namespace Game

#endif