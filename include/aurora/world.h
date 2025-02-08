#ifndef __PROJECT_AURORA_WORLD_HEADER_H__
#define __PROJECT_AURORA_WORLD_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <vector>

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
};

// ---------------------------------------------------

class World
{


public:

};

// ---------------------------------------------------

} // end namespace Game

#endif