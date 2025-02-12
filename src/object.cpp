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

#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__

void StaticObject::render_colliders (const Color& color) const
{
	for (const Collider& collider : this->colliders)
		collider.render(color);
}

#endif

// ---------------------------------------------------


/*void ObjectSprite::render (const float dt)
{
	this->sprite.render();
}*/

// ---------------------------------------------------

/*void ObjectSpriteAnimation::render (const float dt)
{
	this->sprite_animation.render(dt);
}*/

// ---------------------------------------------------

PlayerObject::PlayerObject (World *world_, const Point& pos_)
	: DynamicObject(world_, pos_)
{
	this->colliders.push_back(Collider {
		.object = this,
		.ds = Vector::zero(),
		.size = Vector(2, 2, 2),
		.id = 0
	});
}

// ---------------------------------------------------

void PlayerObject::render (const float dt)
{
#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__
	this->render_colliders(Color::red());
#endif

	const Quaternion q = Quaternion::rotation(Vector(0, 0, -1), Config::camera_vector) * Quaternion::rotation(Vector(0, 1, 0), Vector(1, 1, 0));

	VectorBasis basis = VectorBasis::default_rh_orthonormal_basis();
	basis.rotate(q);

	dprintln("basis = ", basis);

	std::array<GraphicsVertex, 6> graphics_vertices; // 2 triangles

	enum PositionIndex {
		WestSouth = 0,
		EastSouth = 1,
		WestNorth = 2,
		EastSouthRepeat = 3,
		EastNorth = 4,
		WestNorthRepeat = 5
	};

	using enum Rect2D::VertexPositionIndex;

	const Vector half_size = Vector(0.8, 1, 0);

	auto& texture = Texture::tree;
	const Opengl_TextureDescriptor *desc = texture.info->data.get_value<Opengl_TextureDescriptor*>();

	// doing counter clock-wise

	// first triangle - vertices

	graphics_vertices[WestSouth].gvertex.pos = -half_size.x * basis.vx + -half_size.y * basis.vy;
	graphics_vertices[EastSouth].gvertex.pos = half_size.x * basis.vx + -half_size.y * basis.vy;
	graphics_vertices[WestNorth].gvertex.pos = -half_size.x * basis.vx + half_size.y * basis.vy;

	// first triangle - tex coords

	graphics_vertices[WestSouth].tex_coords = Vector(desc->tex_coords[LeftBottom].x, desc->tex_coords[LeftBottom].y, desc->atlas->texture_depth);
	graphics_vertices[EastSouth].tex_coords = Vector(desc->tex_coords[RightBottom].x, desc->tex_coords[RightBottom].y, desc->atlas->texture_depth);
	graphics_vertices[WestNorth].tex_coords = Vector(desc->tex_coords[LeftTop].x, desc->tex_coords[LeftTop].y, desc->atlas->texture_depth);

	// second triangle - vertices

	graphics_vertices[EastSouthRepeat].gvertex.pos = graphics_vertices[EastSouth].gvertex.pos;
	graphics_vertices[EastNorth].gvertex.pos = half_size.x * basis.vx + half_size.y * basis.vy;
	graphics_vertices[WestNorthRepeat].gvertex.pos = graphics_vertices[WestNorth].gvertex.pos;

	// second triangle - tex coords

	graphics_vertices[EastSouthRepeat].tex_coords = graphics_vertices[EastSouth].tex_coords;
	graphics_vertices[EastNorth].tex_coords = Vector(desc->tex_coords[RightTop].x, desc->tex_coords[RightTop].y, desc->atlas->texture_depth);
	graphics_vertices[WestNorthRepeat].tex_coords = graphics_vertices[WestNorth].tex_coords;

	// normals and global positions

	for (auto& gv : graphics_vertices) {
		gv.gvertex.normal = basis.vz;
		gv.offset = this->pos;
	}

	MyGlib::Graphics::Opengl::Renderer *opengl_renderer = static_cast<MyGlib::Graphics::Opengl::Renderer*>(renderer);
	MyGlib::Graphics::Opengl::ProgramTriangleTexture& program = *opengl_renderer->get_program_triangle_texture();
	const uint32_t n_vertices = graphics_vertices.size();
	auto vertices = program.alloc_vertices(n_vertices);

	for (uint32_t i = 0; i < n_vertices; i++)
		vertices[i] = graphics_vertices[i];
}

// ---------------------------------------------------

void PlayerObject::update (const float dt)
{
	constexpr float speed = 2.0;

	if (event_manager->is_key_down(SDL_SCANCODE_RIGHT))
		this->vel.x = speed;
	else if (event_manager->is_key_down(SDL_SCANCODE_LEFT))
		this->vel.x = -speed;
	else
		this->vel.x = 0;
	
	if (event_manager->is_key_down(SDL_SCANCODE_UP))
		this->vel.y = speed;
	else if (event_manager->is_key_down(SDL_SCANCODE_DOWN))
		this->vel.y = -speed;
	else
		this->vel.y = 0;
}

// ---------------------------------------------------

} // end namespace Game