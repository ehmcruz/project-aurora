#ifndef __PROJECT_AURORA_WORLD_HEADER_H__
#define __PROJECT_AURORA_WORLD_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <vector>
#include <memory>

#include <my-lib/std.h>
#include <my-lib/macros.h>
#include <my-lib/matrix.h>

#include <my-game-lib/my-game-lib.h>
#include <my-game-lib/opengl/opengl.h>

#include <aurora/types.h>
#include <aurora/graphics.h>
#include <aurora/object.h>


namespace Game
{

// ---------------------------------------------------

class Map
{
public:
	using GraphicsVertex = MyGlib::Graphics::Opengl::ProgramTriangleTexture::Vertex;

	struct Vertex {
		Vector pos;
	};

private:
	Mylib::Matrix<Vertex> vertices;
	std::vector<GraphicsVertex> graphics_vertices;

public:
	Map ();

	void render ();

	inline Vector2 get_size () const noexcept
	{
		return Vector2(this->vertices.get_ncols() - 1, this->vertices.get_nrows() - 1);
	}
};

// ---------------------------------------------------

class World
{
private:
	std::unique_ptr<Map> map;

	MYLIB_OO_ENCAPSULATE_OBJ_INIT_WITH_COPY_MOVE(Vector, camera_pos, Vector::zero())
	MYLIB_OO_ENCAPSULATE_OBJ_INIT_WITH_COPY_MOVE(Color, ambient_light_color, Color::white())
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(LightPointDescriptor, light)

	// isometric view
	inline static constexpr Vector camera_vector = Vector(1, 1, -1);
	inline static constexpr Vector camera_up = Vector(0, 0, 1);

public:
	World ();

	void render (const float dt);
};

// ---------------------------------------------------

} // end namespace Game

#endif