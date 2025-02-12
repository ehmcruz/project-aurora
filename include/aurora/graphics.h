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
	inline TextureDescriptor main_char;
	inline TextureDescriptor tree_00;
	inline TextureDescriptor castle_00;
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
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(Vector2, size)
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(Vector2, ds)

	std::array<GraphicsVertex, 6> graphics_vertices; // 2 triangles

public:
	Sprite (StaticObject *object_, const TextureDescriptor& texture_, const Vector2 size_, const Vector2 ds_);

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

private:
	Rect2D rect;

public:
	SpriteAnimation (StaticObject *object_, const Rect2D& rect_, std::span<TextureDescriptor> textures, const float frame_duration_);

	void set_scale (const float scale) noexcept;
	void render (const float dt);
};

// ---------------------------------------------------

} // end namespace Game

#endif