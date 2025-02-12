#include <aurora/config.h>
#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/graphics.h>
#include <aurora/object.h>


namespace Game
{

// ---------------------------------------------------

static Quaternion q_basis_rotation;
static Quaternion q_sprite_rotation;
static VectorBasis basis_camera; // z is the camera vector in the opposite direction

// ---------------------------------------------------

static void load_textures ()
{
	using namespace Texture;

	renderer->begin_texture_loading();

	group_grass = renderer->load_texture("assets/grass.png");
	main_char = renderer->load_texture("assets/main-char-walk/GreatSwordKnight_2hWalk_dir1.png");
	tree_00 = renderer->load_texture("assets/tree_00.png");
	castle_00 = renderer->load_texture("assets/castle_00.png");
	
	renderer->end_texture_loading();

	matrix_grass = renderer->split_texture(group_grass, 224 / 16, 400 / 16);
	grass = matrix_grass[2, 2];
}

// ---------------------------------------------------

void load_graphics ()
{
	load_textures();

	q_basis_rotation = Quaternion::rotation(Vector(0, 0, -1), Config::camera_vector) * Quaternion::rotation(Vector(0, 1, 0), Vector(1, 1, 0));
	basis_camera = VectorBasis::default_rh_orthonormal_basis();
	basis_camera.rotate(q_basis_rotation);
}

// ---------------------------------------------------

Sprite::Sprite (StaticObject *object_, const TextureDescriptor& texture_, const Vector2 size_, const Vector2 ds_)
	: object(object_), texture(texture_), size(size_), ds(ds_)
{
	using enum Rect2D::VertexPositionIndex;

	const Vector2 half_size = this->size / fp(2);

	const Opengl_TextureDescriptor *desc = this->texture.info->data.get_value<Opengl_TextureDescriptor*>();

	// doing counter clock-wise

	// first triangle - vertices

	this->graphics_vertices[WestSouth].gvertex.pos = -half_size.x * basis_camera.vx + -half_size.y * basis_camera.vy;
	this->graphics_vertices[EastSouth].gvertex.pos = half_size.x * basis_camera.vx + -half_size.y * basis_camera.vy;
	this->graphics_vertices[WestNorth].gvertex.pos = -half_size.x * basis_camera.vx + half_size.y * basis_camera.vy;

	// first triangle - tex coords

	this->graphics_vertices[WestSouth].tex_coords = Vector(desc->tex_coords[LeftBottom].x, desc->tex_coords[LeftBottom].y, desc->atlas->texture_depth);
	this->graphics_vertices[EastSouth].tex_coords = Vector(desc->tex_coords[RightBottom].x, desc->tex_coords[RightBottom].y, desc->atlas->texture_depth);
	this->graphics_vertices[WestNorth].tex_coords = Vector(desc->tex_coords[LeftTop].x, desc->tex_coords[LeftTop].y, desc->atlas->texture_depth);

	// second triangle - vertices

	this->graphics_vertices[EastSouthRepeat].gvertex.pos = this->graphics_vertices[EastSouth].gvertex.pos;
	this->graphics_vertices[EastNorth].gvertex.pos = half_size.x * basis_camera.vx + half_size.y * basis_camera.vy;
	this->graphics_vertices[WestNorthRepeat].gvertex.pos = this->graphics_vertices[WestNorth].gvertex.pos;

	// second triangle - tex coords

	this->graphics_vertices[EastSouthRepeat].tex_coords = this->graphics_vertices[EastSouth].tex_coords;
	this->graphics_vertices[EastNorth].tex_coords = Vector(desc->tex_coords[RightTop].x, desc->tex_coords[RightTop].y, desc->atlas->texture_depth);
	this->graphics_vertices[WestNorthRepeat].tex_coords = this->graphics_vertices[WestNorth].tex_coords;

	// normals

	for (auto& gv : this->graphics_vertices)
		gv.gvertex.normal = basis_camera.vz;
}

// ---------------------------------------------------

void Sprite::render ()
{
	// update global positions
	for (auto& gv : this->graphics_vertices)
		gv.offset = this->object->get_ref_pos();

	MyGlib::Graphics::Opengl::Renderer *opengl_renderer = static_cast<MyGlib::Graphics::Opengl::Renderer*>(renderer);
	MyGlib::Graphics::Opengl::ProgramTriangleTexture& program = *opengl_renderer->get_program_triangle_texture();
	const uint32_t n_vertices = this->graphics_vertices.size();
	auto vertices = program.alloc_vertices(n_vertices);

	for (uint32_t i = 0; i < n_vertices; i++)
		vertices[i] = graphics_vertices[i];
}

// ---------------------------------------------------

SpriteAnimation::SpriteAnimation (StaticObject *object_, const Rect2D& rect_, std::span<TextureDescriptor> textures, const float frame_duration_)
	: object(object_), frame_duration(frame_duration_), rect(rect_)
{
//	dprintln("SpriteAnimation::SpriteAnimation");
//	dprintln("textures.size() = ", textures.size());

	this->sprites.reserve(textures.size());

//	for (const auto& texture : textures)
//		this->sprites.push_back(Sprite(object_, rect_, texture));
}

// ---------------------------------------------------

void SpriteAnimation::set_scale (const float scale) noexcept
{
//	for (auto& sprite : this->sprites)
//		sprite.set_scale(scale);
}

// ---------------------------------------------------

void SpriteAnimation::render (const float dt)
{
	this->current_frame_time += dt;

	if (this->current_frame_time >= this->frame_duration) {
		this->current_frame_time = 0;
		this->current_frame++;

		if (this->current_frame >= this->sprites.size())
			this->current_frame = 0;
	}

	this->sprites[this->current_frame].render();
}

// ---------------------------------------------------

} // end namespace Game