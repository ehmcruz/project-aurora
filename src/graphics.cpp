#include <aurora/config.h>
#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/graphics.h>
#include <aurora/object.h>


namespace Game
{

// ---------------------------------------------------

static const Quaternion q_rotation_45_degrees = Quaternion::rotation(Vector(0, 1, 0), Vector(1, 1, 0));
static const Quaternion q_rotation_to_camera_vector = Quaternion::rotation(Vector(0, 0, -1), Config::camera_vector);
static const Quaternion q_camera_rotation = q_rotation_to_camera_vector * q_rotation_45_degrees;
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

	basis_camera = VectorBasis::default_rh_orthonormal_basis();
	basis_camera.rotate(q_camera_rotation);
}

// ---------------------------------------------------

/*Sprite::Sprite (StaticObject *object_, const TextureDescriptor& texture_, const Vector2 size_, const Vector2 ds_)
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
}*/

// ---------------------------------------------------

Sprite::Sprite (StaticObject *object_, const TextureDescriptor& texture_, const Vector2 size_, const Vector2 source_anchor_, const Vector3& dest_anchor_)
	: object(object_), texture(texture_) //size(size_), ds(ds_)
{
	using enum Rect2D::VertexPositionIndex;

	const Vector half_size = size_ / fp(2);

	const Opengl_TextureDescriptor *desc = this->texture.info->data.get_value<Opengl_TextureDescriptor*>();

	// First, we set coordinates considering that sprite coordinate (0, 0, 0)
	// is at the center of the sprite.

	this->graphics_vertices[WestSouth].gvertex.pos.x = -half_size.x;
	this->graphics_vertices[WestSouth].gvertex.pos.y = -half_size.y;
	this->graphics_vertices[WestSouth].gvertex.pos.z = 0;

	this->graphics_vertices[EastSouth].gvertex.pos.x = half_size.x;
	this->graphics_vertices[EastSouth].gvertex.pos.y = -half_size.y;
	this->graphics_vertices[EastSouth].gvertex.pos.z = 0;

	this->graphics_vertices[WestNorth].gvertex.pos.x = -half_size.x;
	this->graphics_vertices[WestNorth].gvertex.pos.y = half_size.y;
	this->graphics_vertices[WestNorth].gvertex.pos.z = 0;

	this->graphics_vertices[EastNorth].gvertex.pos.x = half_size.x;
	this->graphics_vertices[EastNorth].gvertex.pos.y = half_size.y;
	this->graphics_vertices[EastNorth].gvertex.pos.z = 0;

	// Now, we need to perform a translation to source_anchor.

	const Vector source_anchor = Vector(source_anchor_.x * size_.x, source_anchor_.y * size_.y, 0);

	this->graphics_vertices[WestSouth].gvertex.pos -= source_anchor;
	this->graphics_vertices[EastSouth].gvertex.pos -= source_anchor;
	this->graphics_vertices[WestNorth].gvertex.pos -= source_anchor;
	this->graphics_vertices[EastNorth].gvertex.pos -= source_anchor;

	// Now, we need to rotate the sprite.
	// First, we rotate the sprite around the z axis 45 degress clock-wise.
	// Then, we rotate the sprite to align it with the camera vector.

#if 1
	this->graphics_vertices[WestSouth].gvertex.pos.rotate(q_camera_rotation);
	this->graphics_vertices[EastSouth].gvertex.pos.rotate(q_camera_rotation);
	this->graphics_vertices[WestNorth].gvertex.pos.rotate(q_camera_rotation);
	this->graphics_vertices[EastNorth].gvertex.pos.rotate(q_camera_rotation);
#endif

#if 0
	this->graphics_vertices[WestSouth].gvertex.pos.rotate(q_rotation_45_degrees);
	this->graphics_vertices[EastSouth].gvertex.pos.rotate(q_rotation_45_degrees);
	this->graphics_vertices[WestNorth].gvertex.pos.rotate(q_rotation_45_degrees);
	this->graphics_vertices[EastNorth].gvertex.pos.rotate(q_rotation_45_degrees);
#endif

#if 0
	this->graphics_vertices[WestSouth].gvertex.pos.rotate(q_rotation_to_camera_vector);
	this->graphics_vertices[EastSouth].gvertex.pos.rotate(q_rotation_to_camera_vector);
	this->graphics_vertices[WestNorth].gvertex.pos.rotate(q_rotation_to_camera_vector);
	this->graphics_vertices[EastNorth].gvertex.pos.rotate(q_rotation_to_camera_vector);
#endif

	// Now, we need to perform a translation to dest_anchor.

	this->graphics_vertices[WestSouth].gvertex.pos += dest_anchor_;
	this->graphics_vertices[EastSouth].gvertex.pos += dest_anchor_;
	this->graphics_vertices[WestNorth].gvertex.pos += dest_anchor_;
	this->graphics_vertices[EastNorth].gvertex.pos += dest_anchor_;

	// copy redundant vertices positions

	this->graphics_vertices[EastSouthRepeat].gvertex.pos = this->graphics_vertices[EastSouth].gvertex.pos;
	this->graphics_vertices[WestNorthRepeat].gvertex.pos = this->graphics_vertices[WestNorth].gvertex.pos;

	// first triangle - tex coords

	this->graphics_vertices[WestSouth].tex_coords = Vector(desc->tex_coords[LeftBottom].x, desc->tex_coords[LeftBottom].y, desc->atlas->texture_depth);
	this->graphics_vertices[EastSouth].tex_coords = Vector(desc->tex_coords[RightBottom].x, desc->tex_coords[RightBottom].y, desc->atlas->texture_depth);
	this->graphics_vertices[WestNorth].tex_coords = Vector(desc->tex_coords[LeftTop].x, desc->tex_coords[LeftTop].y, desc->atlas->texture_depth);

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

	// doing counter clock-wise
	for (uint32_t i = 0; i < n_vertices; i++)
		vertices[i] = graphics_vertices[i];

	if constexpr (Config::render_sprite_box) {
		MyGlib::Graphics::Opengl::ProgramLineColor& program = *opengl_renderer->get_program_line_color();
		constexpr uint32_t n_vertices = 8;
		auto vertices = program.alloc_vertices(n_vertices);

		// counter clock-wise

		vertices[0].gvertex = this->graphics_vertices[WestSouth].gvertex;
		vertices[1].gvertex = this->graphics_vertices[WestNorth].gvertex;
		vertices[2].gvertex = this->graphics_vertices[WestNorth].gvertex;
		vertices[3].gvertex = this->graphics_vertices[EastNorth].gvertex;
		vertices[4].gvertex = this->graphics_vertices[EastNorth].gvertex;
		vertices[5].gvertex = this->graphics_vertices[EastSouth].gvertex;
		vertices[6].gvertex = this->graphics_vertices[EastSouth].gvertex;
		vertices[7].gvertex = this->graphics_vertices[WestSouth].gvertex;

		for (auto& vertex : vertices) {
			vertex.offset = this->object->get_ref_pos();
			vertex.color = Color::blue();
		}
	}
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