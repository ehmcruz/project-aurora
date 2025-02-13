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
};

// ---------------------------------------------------

static std::unordered_map<uint32_t, BluePrintStaticObjectSprite> blueprints_static_object_sprite;

// ---------------------------------------------------

void load_objects ()
{
	// tree_00
	{
		auto [it, success] = blueprints_static_object_sprite.insert({
			std::to_underlying(Object::Subtype::Tree_00),
			{
				.subtype = Object::Subtype::Tree_00,
				.texture = Texture::tree_00,
				.collider_size = Vector3(2, 2, 2),
				.sprite_size = Vector2(2, 2)
			}
		});
		mylib_assert_exception_msg(success, "failed to insert blueprint for tree_00");
	}

	// castle_00
	{
		auto [it, success] = blueprints_static_object_sprite.insert({
			std::to_underlying(Object::Subtype::Castle_00),
			{
				.subtype = Object::Subtype::Castle_00,
				.texture = Texture::castle_00,
				.collider_size = Vector3(5.5, 5, 5),
				.sprite_size = Vector2(7, 7)
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
		Vector2(blueprint.sprite_size.x, blueprint.sprite_size.y),
		Vector2(0, 0)
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
	  sprite(this, Texture::tree_00, Vector2(2, 4), Vector2(0, 0))
{
	this->colliders.push_back(Collider {
		.object = this,
		.ds = Vector::zero(),
		.size = Vector(2, 2, 4),
		.id = 0
	});
}

// ---------------------------------------------------

void PlayerObject::render (const float dt)
{
#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__
	this->render_colliders(Color::red());
#endif

	this->sprite.render();
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

} // end namespace Game