#ifndef __PROJECT_AURORA_WORLD_HEADER_H__
#define __PROJECT_AURORA_WORLD_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <vector>
#include <memory>
#include <list>

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

class World;

// ---------------------------------------------------

class Map : public Object
{
public:
	struct Vertex {
		Point pos;
	};

private:
	Mylib::Matrix<Vertex> vertices;
	std::vector<GraphicsVertex> graphics_vertices;

public:
	Map (World *world_);

	void render (const float dt) override final;
	void update (const float dt) override final;

	float get_z (const Vector2& pos) const noexcept;

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

	std::list< std::unique_ptr<Object> > objects;
	std::list< StaticObject* > static_objects;
	std::list< DynamicObject* > dynamic_objects;

public:
	World ();

	void process_physics (const float dt);
	void process_map_collision ();
	void render (const float dt);
	void process_update (const float dt);

	void add_object (std::unique_ptr<Object> object)
	{
		this->objects.push_back( std::move(object) );
	}

	void add_static_object (std::unique_ptr<StaticObject> object)
	{
		this->static_objects.push_back( object.get() );
		this->add_object( std::move(object) );
	}

	void add_dynamic_object (std::unique_ptr<DynamicObject> object)
	{
		this->dynamic_objects.push_back( object.get() );
		this->add_object( std::move(object) );
	}
};

// ---------------------------------------------------

} // end namespace Game

#endif