#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/graphics.h>
#include <aurora/object.h>
#include <aurora/world.h>


namespace Game
{

// ---------------------------------------------------

Map::Map ()
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

void Map::render ()
{
	MyGlib::Graphics::Opengl::Renderer *opengl_renderer = static_cast<MyGlib::Graphics::Opengl::Renderer*>(renderer);
	MyGlib::Graphics::Opengl::ProgramTriangleTexture& program = *opengl_renderer->get_program_triangle_texture();
	const uint32_t n_vertices = this->graphics_vertices.size();
	auto vertices = program.alloc_vertices(n_vertices);

	for (uint32_t i = 0; i < n_vertices; i++)
		vertices[i] = this->graphics_vertices[i];
}

// ---------------------------------------------------

World::World ()
{
	this->map = std::make_unique<Map>();
	this->camera_pos = Vector(-3, -3, 5);
	this->ambient_light_color.a = 0.5;

	this->light = renderer->add_light_point_source(
		Point(0, 0, 1000), Color::white()
	);
}

// ---------------------------------------------------

void World::render (const float dt)
{
	renderer->setup_render_3D( MyGlib::Graphics::RenderArgs3D {
		.world_camera_pos = this->camera_pos,
		.world_camera_target = this->camera_pos + camera_vector,
		.world_camera_up = camera_up,
		.projection = MyGlib::Graphics::OrthogonalProjectionInfo {
			.view_width = 10,
			.z_near = 0.1,
			.z_far = 100,
		},
		.ambient_light_color = this->ambient_light_color,
		} );

	this->map->render();
}

// ---------------------------------------------------

} // end namespace Game