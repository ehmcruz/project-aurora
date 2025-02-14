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
#include <my-lib/matrix.h>

#include <my-game-lib/my-game-lib.h>

#include <aurora/types.h>


namespace Game
{

// ---------------------------------------------------

class StaticObject;

// ---------------------------------------------------

namespace Texture
{
	inline TextureDescriptor group_grass;
	inline Mylib::Matrix<TextureDescriptor> matrix_grass;
	inline TextureDescriptor grass;
	inline TextureDescriptor tree_00;
	inline TextureDescriptor castle_00;

	inline TextureDescriptor main_char_south;
	inline TextureDescriptor main_char_south_west;
	inline TextureDescriptor main_char_west;
	inline TextureDescriptor main_char_north_west;
	inline TextureDescriptor main_char_north;
	inline TextureDescriptor main_char_north_east;
	inline TextureDescriptor main_char_east;
	inline TextureDescriptor main_char_south_east;

	inline Mylib::Matrix<TextureDescriptor> matrix_main_char_south;
	inline Mylib::Matrix<TextureDescriptor> matrix_main_char_south_west;
	inline Mylib::Matrix<TextureDescriptor> matrix_main_char_west;
	inline Mylib::Matrix<TextureDescriptor> matrix_main_char_north_west;
	inline Mylib::Matrix<TextureDescriptor> matrix_main_char_north;
	inline Mylib::Matrix<TextureDescriptor> matrix_main_char_north_east;
	inline Mylib::Matrix<TextureDescriptor> matrix_main_char_east;
	inline Mylib::Matrix<TextureDescriptor> matrix_main_char_south_east;
}

void load_graphics ();

// ---------------------------------------------------

class Sprite
{
public:
	enum PositionIndex {
		WestSouth = 0,
		EastSouth = 1,
		WestNorth = 2,
		EastSouthRepeat = 3,
		EastNorth = 4,
		WestNorthRepeat = 5
	};

private:
	MYLIB_OO_ENCAPSULATE_PTR_INIT(StaticObject*, object, nullptr)
	MYLIB_OO_ENCAPSULATE_OBJ(TextureDescriptor, texture)
	//MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(Vector2, size)
	//MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(Vector2, ds)

	std::array<GraphicsVertex, 6> graphics_vertices; // 2 triangles

public:
	Sprite (StaticObject *object_, const TextureDescriptor& texture_, const Vector2 size_, const Vector2 source_anchor_, const Vector3& dest_anchor_);

	void render ();
};

// ---------------------------------------------------

class SpriteAnimation
{
private:
	MYLIB_OO_ENCAPSULATE_PTR_INIT(StaticObject*, object, nullptr)
	MYLIB_OO_ENCAPSULATE_OBJ_READONLY(std::vector<Sprite>, sprites)
	MYLIB_OO_ENCAPSULATE_SCALAR(float, frame_duration)
	MYLIB_OO_ENCAPSULATE_SCALAR_INIT(uint32_t, current_frame, 0)
	MYLIB_OO_ENCAPSULATE_SCALAR_INIT(float, current_frame_time, 0)

	bool stopped = false;

public:
	SpriteAnimation () = default;
	SpriteAnimation (StaticObject *object_, std::span<TextureDescriptor> textures, const Vector2 size_, const Vector2 source_anchor_, const Vector3& dest_anchor_, const float frame_duration_);

	void render (const float dt);

	void play () noexcept
	{
		this->stopped = false;
	}
	
	void stop () noexcept
	{
		this->stopped = true;
	}

	void reset () noexcept
	{
		this->current_frame = 0;
		this->current_frame_time = 0;
	}
};

// ---------------------------------------------------

template <uint32_t n, Mylib::Enum T>
class SpriteAnimationArray
{
private:
	MYLIB_OO_ENCAPSULATE_PTR_INIT(StaticObject*, object, nullptr)
	std::array<SpriteAnimation, n> animations;
	T current_animation;

public:
	SpriteAnimationArray(StaticObject *object_,
	                     std::array<std::span<TextureDescriptor>, n> textures,
						 const Vector2 size_,
						 const Vector2 source_anchor_,
						 const Vector3& dest_anchor_,
						 const float frame_duration_,
						 const T initital_animation)
		: object(object_), current_animation(initital_animation)
	{
		for (uint32_t i = 0; i < n; i++)
			this->animations[i] = SpriteAnimation(object_, textures[i], size_, source_anchor_, dest_anchor_, frame_duration_);
	}

	void render (const float dt)
	{
		this->animations[ std::to_underlying(this->current_animation) ].render(dt);
	}

	void set_current_animation (const T animation) noexcept
	{
		if (this->current_animation == animation) {
			this->animations[ std::to_underlying(this->current_animation) ].play();
			return;
		}
		
		this->current_animation = animation;
		this->animations[ std::to_underlying(this->current_animation) ].reset();
		this->animations[ std::to_underlying(this->current_animation) ].play();
	}

	void play () noexcept
	{
		this->animations[ std::to_underlying(this->current_animation) ].play();
	}
	
	void stop () noexcept
	{
		this->animations[ std::to_underlying(this->current_animation) ].stop();
	}
};

// ---------------------------------------------------

} // end namespace Game

#endif