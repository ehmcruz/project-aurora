#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/graphics.h>
#include <aurora/object.h>


namespace Game
{

// ---------------------------------------------------

void load_textures ()
{
	using namespace Texture;

	renderer->begin_texture_loading();

	group_grass = renderer->load_texture("assets/grass.png");
	main_char = renderer->load_texture("assets/main-char-walk/GreatSwordKnight_2hWalk_dir1.png");
	tree = renderer->load_texture("assets/tree.png");
	
	renderer->end_texture_loading();

	matrix_grass = renderer->split_texture(group_grass, 224 / 16, 400 / 16);
	grass = matrix_grass[2, 2];
}

// ---------------------------------------------------

void Sprite::render ()
{
	renderer->draw_rect2D(this->rect, object->get_ref_pos(), TextureRenderOptions { .desc = this->texture });
}

// ---------------------------------------------------

SpriteAnimation::SpriteAnimation (StaticObject *object_, const Rect2D& rect_, std::span<TextureDescriptor> textures, const float frame_duration_)
	: object(object_), frame_duration(frame_duration_), rect(rect_)
{
//	dprintln("SpriteAnimation::SpriteAnimation");
//	dprintln("textures.size() = ", textures.size());

	this->sprites.reserve(textures.size());

	for (const auto& texture : textures)
		this->sprites.push_back(Sprite(object_, rect_, texture));
}

// ---------------------------------------------------

void SpriteAnimation::set_scale (const float scale) noexcept
{
	for (auto& sprite : this->sprites)
		sprite.set_scale(scale);
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