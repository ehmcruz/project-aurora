#include <limits>
#include <iterator>

#include <aurora/config.h>
#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/graphics.h>
#include <aurora/audio.h>
#include <aurora/object.h>
#include <aurora/world.h>


namespace Game
{

// ---------------------------------------------------

Map::Map (World *world_)
	: Object(world_, Subtype::Map)
{
	constexpr uint32_t map_tile_n = 20;

	constexpr char vertex_map[(map_tile_n+1)*(map_tile_n+1)+1] =
		"               123333"
		"               123333"
		"               123333"
		"               122222"
		"               111111"
		"                ---  "
		"                ---  "
		"                ---  "
		"                ---  "
		"                ---  "
		"                ---  "
		"                ---  "
		"                ---  "
		"               ---   "
		"              ---    "
		"             ---     "
		"            ---      "
		"            ---      "
		"            ---      "
		"            ---      "
		"            ---      ";

	constexpr char tile_map[(map_tile_n)*(map_tile_n)+1] =
		"                    "
		"                    "
		"                    "
		"                    "
		"                    "
		"                ww  "
		"                ww  "
		"                ww  "
		"                ww  "
		"                ww  "
		"                ww  "
		"                ww  "
		"               www  "
		"              www   "
		"             www    "
		"            www     "
		"            ww      "
		"            ww      "
		"            ww      "
		"            ww      " ;

	this->vertices = Mylib::Matrix<Vertex>(map_tile_n+1, map_tile_n+1);
	this->tiles = Mylib::Matrix<Tile>(map_tile_n, map_tile_n);

	// create terrain mesh

	auto get_vertex_altitude = [vertex_map] (const uint32_t j, const uint32_t i_) -> float {
		const uint32_t i = map_tile_n - i_;
		const char c = vertex_map[i*(map_tile_n+1) + j];
		
		if (c == ' ')
			return 0.0f;
		else if (c == '-')
			return -1.0f;
		else
			return static_cast<float>(c - '0');
	};

	auto get_tile_texture = [tile_map] (const uint32_t j, const uint32_t i_) -> TextureDescriptor& {
		const uint32_t i = map_tile_n - i_ - 1;
		const char c = tile_map[i*map_tile_n + j];
		
		if (c == 'w')
			return Texture::water;
		else
			return Texture::grass;
	};

	for (uint32_t i = 0; i < this->vertices.get_nrows(); i++) {
		for (uint32_t j = 0; j < this->vertices.get_ncols(); j++) {
			this->vertices[i, j] = Vertex {
				.pos = Vector(i, j, get_vertex_altitude(i, j))
			};
		}
	}

	// generate graphics vertices

	using enum Rect2D::VertexPositionIndex;

	this->graphics_vertices.resize((this->vertices.get_nrows()-1) * (this->vertices.get_ncols()-1) * 6);

	uint32_t k = 0;
	for (uint32_t i = 0; i < this->vertices.get_nrows()-1; i++) {
		for (uint32_t j = 0; j < this->vertices.get_ncols()-1; j++) {
			GraphicsVertex *gv = &this->graphics_vertices[k];

			for (uint32_t l = 0; l < 6; l++) {
				gv[l].gvertex.pos = Vector::zero(); // local position
			}

			enum PositionIndex {
				WestSouth = 0,
				EastSouth = 1,
				WestNorth = 2,
				EastSouthRepeat = 3,
				EastNorth = 4,
				WestNorthRepeat = 5
			};

			// select texture from tile map

			const Opengl_TextureDescriptor *desc = get_tile_texture(i, j).info->data.get_value<Opengl_TextureDescriptor*>();

			// doing counter clock-wise

			// first triangle

			gv[WestSouth].offset = this->vertices[i, j].pos;
			gv[EastSouth].offset = this->vertices[i+1, j].pos;
			gv[WestNorth].offset = this->vertices[i, j+1].pos;

			gv[WestSouth].tex_coords = Vector(desc->tex_coords[LeftBottom].x, desc->tex_coords[LeftBottom].y, desc->atlas->texture_depth);
			gv[EastSouth].tex_coords = Vector(desc->tex_coords[RightBottom].x, desc->tex_coords[RightBottom].y, desc->atlas->texture_depth);
			gv[WestNorth].tex_coords = Vector(desc->tex_coords[LeftTop].x, desc->tex_coords[LeftTop].y, desc->atlas->texture_depth);

			// second triangle

			gv[EastSouthRepeat].offset = gv[EastSouth].offset;
			gv[EastNorth].offset = this->vertices[i+1, j+1].pos;
			gv[WestNorthRepeat].offset = gv[WestNorth].offset;

			gv[EastSouthRepeat].tex_coords = gv[EastSouth].tex_coords;
			gv[EastNorth].tex_coords = Vector(desc->tex_coords[RightTop].x, desc->tex_coords[RightTop].y, desc->atlas->texture_depth);
			gv[WestNorthRepeat].tex_coords = gv[WestNorth].tex_coords;

			// calculate normals

			Vector dir_east = gv[EastSouth].offset - gv[WestSouth].offset;
			Vector dir_north = gv[WestNorth].offset - gv[WestSouth].offset;
			gv[WestSouth].gvertex.normal = Mylib::Math::normalize( Mylib::Math::cross_product(dir_east, dir_north) );
			gv[EastSouth].gvertex.normal = gv[WestSouth].gvertex.normal;
			gv[WestNorth].gvertex.normal = gv[WestSouth].gvertex.normal;

			dir_east = gv[EastNorth].offset - gv[WestNorth].offset;
			dir_north = gv[EastNorth].offset - gv[EastSouth].offset;
			gv[EastSouthRepeat].gvertex.normal = Mylib::Math::normalize( Mylib::Math::cross_product(dir_east, dir_north) );
			gv[EastNorth].gvertex.normal = gv[EastSouthRepeat].gvertex.normal;
			gv[WestNorthRepeat].gvertex.normal = gv[EastSouthRepeat].gvertex.normal;

			k += 6;
		}
	}
}

// ---------------------------------------------------

void Map::render (const float dt)
{
	MyGlib::Graphics::Opengl::Renderer *opengl_renderer = static_cast<MyGlib::Graphics::Opengl::Renderer*>(renderer);
	MyGlib::Graphics::Opengl::ProgramTriangleTexture& program = *opengl_renderer->get_program_triangle_texture();
	const uint32_t n_vertices = this->graphics_vertices.size();
	auto vertices = program.alloc_vertices(n_vertices);

	for (uint32_t i = 0; i < n_vertices; i++)
		vertices[i] = this->graphics_vertices[i];
}

// ---------------------------------------------------

void Map::update (const float dt)
{
}

// ---------------------------------------------------

float Map::get_z (const Vector2& pos) const noexcept
{
	const Vector2 map_size = this->get_size();

	if (pos.x < 0 || pos.y < 0 || pos.x >= map_size.x || pos.y >= map_size.y) [[unlikely]]
		return std::numeric_limits<float>::lowest();

	const uint32_t cell_x = pos.x;
	const uint32_t cell_y = pos.y;
	const Vector2 local_pos = Vector2(pos.x, pos.y) - Vector2(cell_x, cell_y);

	/*
		f(x) = -x + 1

		if (local_pos.y < f(local_pos.x)), then we are in the WestSouth triangle
		else, we are in the EastNorth triangle.
	*/

	auto f = [](const float x) -> float {
		return -x + 1;
	};

	Plane plane;

	// let's create a plane from the triangle

	if (local_pos.y < f(local_pos.x)) { // WestSouth triangle
		plane.point = this->vertices[cell_x, cell_y].pos;
		plane.normal = Mylib::Math::normalize( Mylib::Math::cross_product(
			this->vertices[cell_x+1, cell_y].pos - this->vertices[cell_x, cell_y].pos,
			this->vertices[cell_x, cell_y+1].pos - this->vertices[cell_x, cell_y].pos
		) );
	}
	else { // EastNorth triangle
		plane.point = this->vertices[cell_x+1, cell_y+1].pos;
		plane.normal = Mylib::Math::normalize( Mylib::Math::cross_product(
			this->vertices[cell_x, cell_y+1].pos - this->vertices[cell_x+1, cell_y+1].pos,
			this->vertices[cell_x+1, cell_y].pos - this->vertices[cell_x+1, cell_y+1].pos
		) );
	}

	// calculate the z value

	const auto line = Line {
		.point = Point(pos.x, pos.y, 0),
		.vector = Vector(0, 0, 1)
	};

	const Point intersection = Mylib::Math::intersection(plane, line);

	return intersection.z;
}

// ---------------------------------------------------

World::World ()
{
	this->map = std::make_unique<Map>(this);
	this->camera_pos = Vector(-3, -3, 5);
	this->ambient_light_color.a = 0;

	this->light = renderer->add_light_point_source(
		Point(-10, -10, 100), Color::white()
	);

	this->add_static_object_at_ground( build_static_object_sprite(this, Object::Subtype::Castle_00, Point(10, 10, foo)) );
	this->add_static_object_at_ground( build_static_object_sprite(this, Object::Subtype::Tree_00, Point(1, 9, foo)) );
	this->add_static_object_at_ground( build_static_object_sprite(this, Object::Subtype::Tree_00, Point(3, 3, foo)) );
	this->add_static_object_at_ground( build_static_object_sprite(this, Object::Subtype::Tree_00, Point(18, 2, foo)) );
	this->add_static_object_at_ground( build_static_object_sprite(this, Object::Subtype::Tree_00, Point(5, 16, foo)) );
	this->add_object( std::make_unique<EnemyObject>(this, std::initializer_list<Point2> { Point2(1, 1), Point2(3, 1) } ) );
	this->add_object( std::make_unique<EnemyObject>(this, std::initializer_list<Point2> { Point2(1, 10), Point2(6, 10) } ) );
	this->player = static_cast<PlayerObject*>( this->add_object( std::make_unique<PlayerObject>(this, Vector(1, 1, 3)) ) );

	audio_manager->play_audio(Audio::background_music);
}

// ---------------------------------------------------

void World::process_physics (const float dt) noexcept
{
	for (DynamicObject *obj : this->dynamic_objects)
		obj->physics(dt);

	this->process_map_collision();
	this->process_object_collision();
}

// ---------------------------------------------------

void World::process_map_collision () noexcept
{
	for (DynamicObject *obj : this->dynamic_objects) {
		Vector& obj_pos = obj->get_ref_pos();

		for (Collider& collider : obj->get_colliders()) {
			const Point collider_pos = obj_pos + collider.ds;
			const float z = this->map->get_z(Vector2(collider_pos.x, collider_pos.y));
			const float collider_lowest_z = collider_pos.z - collider.size.z / fp(2);
			const float altitude = collider_lowest_z - z;

			if (altitude < 0) {
				obj->get_ref_vel().z = 0;
				obj_pos.z -= altitude;
			}
		}
	}
}

// ---------------------------------------------------

void World::process_object_collision () noexcept
{
	// Dynamic objects to static objects
	
	for (DynamicObject *d_obj : this->dynamic_objects) {
		for (Collider& d_collider : d_obj->get_colliders()) {
			for (StaticObject *s_obj : this->static_objects) {
				for (Collider& s_collider : s_obj->get_colliders()) {
					const auto [colliding, ds] = check_collision(s_collider, d_collider);
					const auto abs_ds = Mylib::Math::abs(ds);

					if (colliding) {
						if (abs_ds.x <= abs_ds.y && abs_ds.x <= abs_ds.z) {
							d_obj->get_ref_vel().x = 0;
							d_obj->get_ref_pos().x += ds.x;
						}
						else if (abs_ds.y <= abs_ds.x && abs_ds.y <= abs_ds.z) {
							d_obj->get_ref_vel().y = 0;
							d_obj->get_ref_pos().y += ds.y;
						}
						else {
							d_obj->get_ref_vel().z = 0;
							d_obj->get_ref_pos().z += ds.z;
						}

						d_obj->collision(d_collider, s_collider, ds);
						s_obj->collision(s_collider, d_collider, ds);
					}
				}
			}
		}
	}

	// Dynamic objects to dynamic objects
	
	for (auto it_a = this->dynamic_objects.begin(); it_a != this->dynamic_objects.end(); it_a++) {
		DynamicObject *obj_a = *it_a;

		for (auto it_b = std::next(it_a); it_b != this->dynamic_objects.end(); it_b++) {
			DynamicObject *obj_b = *it_b;

			for (Collider& collider_a : obj_a->get_colliders()) {
				for (Collider& collider_b : obj_b->get_colliders()) {
					const auto [colliding, ds] = check_collision(collider_a, collider_b);
					const auto abs_ds = Mylib::Math::abs(ds);

					if (colliding) {
						if (abs_ds.x <= abs_ds.y && abs_ds.x <= abs_ds.z) {
							obj_a->get_ref_pos().x -= ds.x / fp(2);
							obj_b->get_ref_pos().x += ds.x / fp(2);
						}
						else if (abs_ds.y <= abs_ds.x && abs_ds.y <= abs_ds.z) {
							obj_a->get_ref_pos().y -= ds.y / fp(2);
							obj_b->get_ref_pos().y += ds.y / fp(2);
						}
						else {
							obj_a->get_ref_pos().z -= ds.z / fp(2);
							obj_b->get_ref_pos().z += ds.z / fp(2);
						}

						obj_a->collision(collider_a, collider_b, ds);
						obj_b->collision(collider_b, collider_a, ds);
					}
				}
			}
		}
	}
}

// ---------------------------------------------------

void World::render (const float dt)
{
	this->camera_pos = this->player->get_ref_pos() - Config::camera_vector * fp(50);

	renderer->setup_render_3D( MyGlib::Graphics::RenderArgs3D {
		.world_camera_pos = this->camera_pos,
		.world_camera_target = this->player->get_ref_pos(),
		.world_camera_up = Config::camera_up,
		.projection = MyGlib::Graphics::OrthogonalProjectionInfo {
			.view_width = 10,
			.z_near = 0.1,
			.z_far = 100,
		},
		.ambient_light_color = this->ambient_light_color,
		} );

	this->map->render(dt);

	for (auto& obj : this->objects)
		obj->render(dt);
}

// ---------------------------------------------------

void World::process_update (const float dt)
{
	for (auto& obj : this->objects)
		obj->update(dt);
}

// ---------------------------------------------------

Object* World::add_object (std::unique_ptr<Object> object)
{
	Object *obj = object.get();
	this->objects.push_back( std::move(object) );

	// careful since a dynamic object is also a static object

	if (DynamicObject *d_obj = dynamic_cast<DynamicObject*>(obj))
		this->dynamic_objects.push_back(d_obj);
	else if (StaticObject *s_obj = dynamic_cast<StaticObject*>(obj))
		this->static_objects.push_back(s_obj);

	return obj;
}

// ---------------------------------------------------

StaticObject* World::add_static_object_at_ground (std::unique_ptr<StaticObject> object)
{
	const float floor_z = this->map->get_z(Vector2(object->get_value_pos().x, object->get_value_pos().y));

	Vector& obj_pos = object->get_ref_pos();
	const Collider *lowest_z_collider = nullptr;
	float lowest_z = std::numeric_limits<float>::max(); // in object local coordinates

	for (const Collider& collider : object->get_colliders()) {
		const float collider_z = obj_pos.z + collider.ds.z - collider.size.z / fp(2);

		if (collider_z < lowest_z) {
			lowest_z = collider_z;
			lowest_z_collider = &collider;
		}
	}

	obj_pos.z += floor_z - lowest_z;

	return static_cast<StaticObject*>( this->add_object(std::move(object)) );
}

// ---------------------------------------------------

void World::frame_finished ()
{
	for (Object *obj : this->objects_to_remove_next_frame) {
		// careful since a dynamic object is also a static object

		if (DynamicObject *d_obj = dynamic_cast<DynamicObject*>(obj))
			this->dynamic_objects.remove(d_obj);
		else if (StaticObject *s_obj = dynamic_cast<StaticObject*>(obj))
			this->static_objects.remove(s_obj);
		
		this->objects.remove_if([obj](const std::unique_ptr<Object>& ptr) -> bool {
			return ptr.get() == obj;
		});
	}

	this->objects_to_remove_next_frame.clear();
}

// ---------------------------------------------------

} // end namespace Game