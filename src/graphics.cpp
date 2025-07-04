#include <aurora/config.h>
#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/graphics.h>
#include <aurora/object.h>


namespace Game
{

// ---------------------------------------------------

// Some math functions will be constexpr only in c++26.
// However, in several of them GCC already supports constexpr.
// But clang does not support constexpr yet on them.
// When both GCC and clang support c++26 math constexpr, we can remove this macro.

#ifdef __clang__
	#define XX_CONSTEXPR const
#elif defined(__GNUC__)
	#define XX_CONSTEXPR constexpr
#else
	#define XX_CONSTEXPR const
#endif

static XX_CONSTEXPR Quaternion q_rotation_45_degrees = Quaternion::rotation(Vector(0, 1, 0), Vector(1, 1, 0));
static XX_CONSTEXPR Quaternion q_rotation_to_camera_vector = Quaternion::rotation(Vector(0, 0, -1), Config::camera_vector);
static XX_CONSTEXPR Quaternion q_camera_rotation = q_rotation_to_camera_vector * q_rotation_45_degrees;
static VectorBasis basis_camera; // z is the camera vector in the opposite direction

#undef XX_CONSTEXPR

// ---------------------------------------------------

static void load_textures ()
{
	using namespace Texture;

	renderer->begin_texture_loading();

	tree_00 = renderer->load_texture("assets/tree_00.png");
	castle_00 = renderer->load_texture("assets/castle_00.png");

	main_char_south = renderer->load_texture("assets/main-char-walk/GreatSwordKnight_2hWalk_dir_south-cropped.png");
	main_char_south_west = renderer->load_texture("assets/main-char-walk/GreatSwordKnight_2hWalk_dir_south_west-cropped.png");
	main_char_west = renderer->load_texture("assets/main-char-walk/GreatSwordKnight_2hWalk_dir_west-cropped.png");
	main_char_north_west = renderer->load_texture("assets/main-char-walk/GreatSwordKnight_2hWalk_dir_north_west-cropped.png");
	main_char_north = renderer->load_texture("assets/main-char-walk/GreatSwordKnight_2hWalk_dir_north-cropped.png");
	main_char_north_east = renderer->load_texture("assets/main-char-walk/GreatSwordKnight_2hWalk_dir_north_east-cropped.png");
	main_char_east = renderer->load_texture("assets/main-char-walk/GreatSwordKnight_2hWalk_dir_east-cropped.png");
	main_char_south_east = renderer->load_texture("assets/main-char-walk/GreatSwordKnight_2hWalk_dir_south_east-cropped.png");
	grass = renderer->load_texture("assets/grass.png");
	water = renderer->load_texture("assets/water.png");
	enemy_00 = renderer->load_texture("assets/enemy_00.png");
	explosion = renderer->load_texture("assets/explosion_001S.png");
	
	renderer->end_texture_loading();

	matrix_main_char_south = renderer->split_texture(main_char_south, 3, 3);
	matrix_main_char_south_west = renderer->split_texture(main_char_south_west, 3, 3);
	matrix_main_char_west = renderer->split_texture(main_char_west, 3, 3);
	matrix_main_char_north_west = renderer->split_texture(main_char_north_west, 3, 3);
	matrix_main_char_north = renderer->split_texture(main_char_north, 3, 3);
	matrix_main_char_north_east = renderer->split_texture(main_char_north_east, 3, 3);
	matrix_main_char_east = renderer->split_texture(main_char_east, 3, 3);
	matrix_main_char_south_east = renderer->split_texture(main_char_south_east, 3, 3);

	matrix_explosion = renderer->split_texture(explosion, 3, 3);
}

// ---------------------------------------------------

void load_graphics ()
{
	load_textures();

	basis_camera = VectorBasis::default_rh_orthonormal_basis();
	basis_camera.rotate(q_camera_rotation);
}

// ---------------------------------------------------

Sprite::Sprite (StaticObject *object_, const TextureDescriptor& texture_, const Vector2 size_, const Vector2 source_anchor_, const Vector3& dest_anchor_)
	: object(object_), texture(texture_) //size(size_), ds(ds_)
{
	using enum MyGlib::Graphics::Enums::TextureVertexPositionIndex;

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
			vertex.color = Colors::blue;
		}
	}
}

// ---------------------------------------------------

SpriteAnimation::SpriteAnimation (StaticObject *object_, std::span<TextureDescriptor> textures, const Vector2 size_, const Vector2 source_anchor_, const Vector3& dest_anchor_, const float frame_duration_)
	: object(object_), frame_duration(frame_duration_)
{
	this->sprites.reserve(textures.size());

	for (const auto& texture : textures)
		this->sprites.push_back(Sprite(object_, texture, size_, source_anchor_, dest_anchor_));
}

// ---------------------------------------------------

void SpriteAnimation::render (const float dt)
{
	this->sprites[this->current_frame].render();

	if (this->stopped == false) {
		this->current_frame_time += dt;

		if (this->current_frame_time >= this->frame_duration) {
			this->current_frame_time = 0;
			this->current_frame++;

			if (this->current_frame >= this->sprites.size()) {
				this->current_frame = 0;
				this->event_handler.publish(0);
			}
		}
	}
}

// ---------------------------------------------------

} // end namespace Game