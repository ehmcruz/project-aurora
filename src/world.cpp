#include <limits>

#include <aurora/config.h>
#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/graphics.h>
#include <aurora/object.h>
#include <aurora/world.h>


namespace Game
{

// ---------------------------------------------------

Map::Map (World *world_)
	: Object(world_, Subtype::Map)
{
	this->vertices = Mylib::Matrix<Vertex>(100, 100);

	for (uint32_t i = 0; i < this->vertices.get_nrows(); i++) {
		for (uint32_t j = 0; j < this->vertices.get_ncols(); j++) {
			this->vertices[i, j] = Vertex {
				.pos = Vector(i, j, 0)
			};
		}
	}

	// generate graphics vertices

	using enum Rect2D::VertexPositionIndex;

	const Opengl_TextureDescriptor *desc = Texture::grass.info->data.get_value<Opengl_TextureDescriptor*>();

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

			// doing counter clock-wise

			// first triangle

			gv[WestSouth].offset = Vector(i, j, 0);
			gv[EastSouth].offset = Vector(i+1, j, 0);
			gv[WestNorth].offset = Vector(i, j+1, 0);

			gv[WestSouth].tex_coords = Vector(desc->tex_coords[LeftBottom].x, desc->tex_coords[LeftBottom].y, desc->atlas->texture_depth);
			gv[EastSouth].tex_coords = Vector(desc->tex_coords[RightBottom].x, desc->tex_coords[RightBottom].y, desc->atlas->texture_depth);
			gv[WestNorth].tex_coords = Vector(desc->tex_coords[LeftTop].x, desc->tex_coords[LeftTop].y, desc->atlas->texture_depth);

			// second triangle

			gv[EastSouthRepeat].offset = gv[EastSouth].offset;
			gv[EastNorth].offset = Vector(i+1, j+1, 0);
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
	this->ambient_light_color.a = 0.5;

	this->light = renderer->add_light_point_source(
		Point(0, 0, 1000), Color::white()
	);

	this->add_static_object_at_ground( build_static_object_sprite(this, Object::Subtype::Castle_00, Point(5, 5, foo)) );
	this->add_static_object_at_ground( build_static_object_sprite(this, Object::Subtype::Tree_00, Point(1, 9, foo)) );
	this->add_static_object_at_ground( build_static_object_sprite(this, Object::Subtype::Tree_00, Point(3, 9, foo)) );
	this->add_dynamic_object( std::make_unique<PlayerObject>(this, Vector(5, 2, 10)) );
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
	for (DynamicObject *d_obj : this->dynamic_objects) {
		for (Collider& d_collider : d_obj->get_colliders()) {
			for (StaticObject *s_obj : this->static_objects) {
				for (Collider& s_collider : s_obj->get_colliders()) {
					const auto [colliding, ds] = check_collision(s_collider, d_collider);
					const auto abs_ds = Mylib::Math::abs(ds);

					if (colliding) {
						if (abs_ds.x < abs_ds.y && abs_ds.x < abs_ds.z) {
							d_obj->get_ref_vel().x = 0;
							d_obj->get_ref_pos().x += ds.x;
						}
						else if (abs_ds.y < abs_ds.x && abs_ds.y < abs_ds.z) {
							d_obj->get_ref_vel().y = 0;
							d_obj->get_ref_pos().y += ds.y;
						}
						else {
							d_obj->get_ref_vel().z = 0;
							d_obj->get_ref_pos().z += ds.z;
						}
					}
				}
			}
		}
	}
}

// ---------------------------------------------------

void World::render (const float dt)
{
	renderer->setup_render_3D( MyGlib::Graphics::RenderArgs3D {
		.world_camera_pos = this->camera_pos,
		.world_camera_target = this->camera_pos + Config::camera_vector,
		.world_camera_up = Config::camera_up,
		.projection = MyGlib::Graphics::OrthogonalProjectionInfo {
			.view_width = 15,
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

void World::add_static_object_at_ground (std::unique_ptr<StaticObject> object)
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

	this->add_static_object(std::move(object));
}

// ---------------------------------------------------

} // end namespace Game