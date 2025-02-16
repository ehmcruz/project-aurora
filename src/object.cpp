#include <unordered_map>
#include <utility>

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

struct BluePrintStaticObjectSprite {
	Object::Subtype subtype;
	TextureDescriptor& texture;
	Vector3 collider_size;
	Vector2 sprite_size;
	Vector2 sprite_source_anchor;
	Vector3 sprite_dest_anchor;
};

// ---------------------------------------------------

static std::unordered_map<uint32_t, BluePrintStaticObjectSprite> blueprints_static_object_sprite;

// ---------------------------------------------------

void load_objects ()
{
	// tree_00
	{
		const auto [it, success] = blueprints_static_object_sprite.insert({
			std::to_underlying(Object::Subtype::Tree_00),
			{
				.subtype = Object::Subtype::Tree_00,
				.texture = Texture::tree_00,
				.collider_size = Vector3(1, 1, 4),
				.sprite_size = Vector2(4, 4),
				.sprite_source_anchor = Vector2(0, -0.5),
				.sprite_dest_anchor = Vector3(0, 0, -2)
			}
		});
		mylib_assert_exception_msg(success, "failed to insert blueprint for tree_00");
	}

	// castle_00
	{
		const auto [it, success] = blueprints_static_object_sprite.insert({
			std::to_underlying(Object::Subtype::Castle_00),
			{
				.subtype = Object::Subtype::Castle_00,
				.texture = Texture::castle_00,
				.collider_size = Vector3(5, 5, 5),
				.sprite_size = Vector2(6, 6),
				.sprite_source_anchor = Vector2(0, -0.5),
				.sprite_dest_anchor = Vector3(-2.5, -2.5, -2.5)
			}
		});
		mylib_assert_exception_msg(success, "failed to insert blueprint for castle_00");
	}
}

// ---------------------------------------------------

std::unique_ptr<StaticObjectSprite> build_static_object_sprite (
	World *world,
	const Object::Subtype subtype,
	const Vector& pos
	)
{
	const auto it = blueprints_static_object_sprite.find({std::to_underlying(subtype)});
	mylib_assert_exception_msg(it != blueprints_static_object_sprite.end(), "blueprint not found for ", subtype);

	const auto& blueprint = it->second;

	std::unique_ptr<StaticObjectSprite> r = std::make_unique<StaticObjectSprite>(
		world,
		blueprint.subtype,
		pos,
		blueprint.texture,
		blueprint.sprite_size,
		blueprint.sprite_source_anchor,
		blueprint.sprite_dest_anchor
	);

	r->get_colliders().push_back(Collider {
		.object = r.get(),
		.ds = Vector::zero(),
		.size = blueprint.collider_size,
		.id = 0
	});

	return r;
}

// ---------------------------------------------------

#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__

void StaticObject::render_colliders (const Color& color) const
{
	for (const Collider& collider : this->colliders)
		collider.render(color);
}

#endif

// ---------------------------------------------------

void StaticObjectSprite::render (const float dt)
{
#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__
	this->render_colliders(Color::red());
#endif

	this->sprite.render();
}

// ---------------------------------------------------

void StaticObjectSprite::update (const float dt)
{
}

// ---------------------------------------------------

PlayerObject::PlayerObject (World *world_, const Point& pos_)
	: DynamicObject(world_, Subtype::Player, pos_),
	  animations(
	      this,
	      {
	          Texture::matrix_main_char_south.to_span(),
			  Texture::matrix_main_char_south_west.to_span(),
			  Texture::matrix_main_char_west.to_span(),
			  Texture::matrix_main_char_north_west.to_span(),
			  Texture::matrix_main_char_north.to_span(),
			  Texture::matrix_main_char_north_east.to_span(),
			  Texture::matrix_main_char_east.to_span(),
			  Texture::matrix_main_char_south_east.to_span()
	      },
	      Vector2(0.7, 0.7),
	      Vector2(0, -0.5),
	      Vector3(0, 0, -0.35),
	      0.05,
	      Direction::South
	  )
{
	this->colliders.push_back(Collider {
		.object = this,
		.ds = Vector::zero(),
		.size = Vector(0.3, 0.3, 0.7),
		.id = 0
	});
}

// ---------------------------------------------------

void PlayerObject::render (const float dt)
{
#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__
	this->render_colliders(Color::red());
#endif

	this->animations.render(dt);
}

// ---------------------------------------------------

void PlayerObject::update (const float dt)
{
	constexpr float speed = 5.0;

	constexpr auto key_down = SDL_SCANCODE_DOWN;
	constexpr auto key_up = SDL_SCANCODE_UP;
	constexpr auto key_left = SDL_SCANCODE_LEFT;
	constexpr auto key_right = SDL_SCANCODE_RIGHT;

	bool stop = false;

	if (event_manager->is_key_down(key_down)) {
		if (event_manager->is_key_down(key_right))
			this->direction = Direction::SouthEast;
		else if (event_manager->is_key_down(key_left))
			this->direction = Direction::SouthWest;
		else
			this->direction = Direction::South;
	}
	else if (event_manager->is_key_down(key_up)) {
		if (event_manager->is_key_down(key_right))
			this->direction = Direction::NorthEast;
		else if (event_manager->is_key_down(key_left))
			this->direction = Direction::NorthWest;
		else
			this->direction = Direction::North;
	}
	else {
		if (event_manager->is_key_down(key_right))
			this->direction = Direction::East;
		else if (event_manager->is_key_down(key_left))
			this->direction = Direction::West;
		else {
			// keep last direction
			stop = true;
		}
	}

	constexpr std::array<Vector2, 8> velocity_dir = {
		Vector2(-1, -1),    // South
		Vector2(-1, 0),     // SouthWest
		Vector2(-1, 1),     // West
		Vector2(0, 1),      // NorthWest
		Vector2(1, 1),      // North
		Vector2(1, 0),      // NorthEast
		Vector2(1, -1),     // East
		Vector2(0, -1)      // SouthEast
	};

	if (!stop) {
		const Vector2 new_vel = Mylib::Math::with_length( velocity_dir[ std::to_underlying(this->direction) ], speed);
		this->vel.x = new_vel.x;
		this->vel.y = new_vel.y;

		this->animations.set_current_animation(this->direction);
	}
	else {
		this->vel.x = 0;
		this->vel.y = 0;

		this->animations.stop();
	}
	
	
}

// ---------------------------------------------------

const char* enum_class_to_str (const Object::Type value)
{
	static constexpr auto strs = std::to_array<const char*>({
		#define _MYLIB_ENUM_CLASS_OBJECT_TYPE_VALUE_(V) #V,
		_MYLIB_ENUM_CLASS_OBJECT_TYPE_VALUES_
		#undef _MYLIB_ENUM_CLASS_OBJECT_TYPE_VALUE_
	});

	mylib_assert_exception_msg(std::to_underlying(value) < strs.size(), "invalid enum class value ", std::to_underlying(value))

	return strs[ std::to_underlying(value) ];
}

// ---------------------------------------------------

const char* enum_class_to_str (const Object::Subtype value)
{
	static constexpr auto strs = std::to_array<const char*>({
		#define _MYLIB_ENUM_CLASS_OBJECT_SUBTYPE_VALUE_(TYPE, V) #V,
		_MYLIB_ENUM_CLASS_OBJECT_SUBTYPE_VALUES_
		#undef _MYLIB_ENUM_CLASS_OBJECT_SUBTYPE_VALUE_
	});

	mylib_assert_exception_msg(std::to_underlying(value) < strs.size(), "invalid enum class value ", std::to_underlying(value))

	return strs[ std::to_underlying(value) ];
}

// ---------------------------------------------------

const char* enum_class_to_str (const Object::Direction value)
{
	static constexpr auto strs = std::to_array<const char*>({
		#define _MYLIB_ENUM_CLASS_OBJECT_TYPE_VALUE_(V) #V,
		_MYLIB_ENUM_CLASS_OBJECT_TYPE_VALUES_
		#undef _MYLIB_ENUM_CLASS_OBJECT_TYPE_VALUE_
	});

	mylib_assert_exception_msg(std::to_underlying(value) < strs.size(), "invalid enum class value ", std::to_underlying(value))

	return strs[ std::to_underlying(value) ];
}

// ---------------------------------------------------

} // end namespace Game