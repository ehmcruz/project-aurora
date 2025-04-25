#include <unordered_map>
#include <utility>
#include <numbers>
#include <numeric>

#include <cmath>

#include <boost/container/static_vector.hpp>

#include <my-lib/std.h>
#include <my-lib/math.h>
#include <my-lib/utils.h>

#include <aurora/config.h>
#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/graphics.h>
#include <aurora/object.h>
#include <aurora/world.h>
#include <aurora/audio.h>


namespace Game
{

// ---------------------------------------------------

namespace Config
{
	inline constexpr float spell_color_time = 0.5f;
	inline constexpr float spell_size = 0.2f;
	inline constexpr float spell_speed = 4.0f;
	inline constexpr float spell_angular_speed = Mylib::Math::degrees_to_radians(270.0f);
	inline constexpr float spell_life_span = 2.0f;
}

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

struct BluePrintStaticObjectAnimation {
	Object::Subtype subtype;
	std::span<TextureDescriptor> textures;
	Vector3 collider_size;
	Vector2 sprite_size;
	Vector2 sprite_source_anchor;
	Vector3 sprite_dest_anchor;
	float frame_duration;
	bool die_after_animation;
};

// ---------------------------------------------------

static std::unordered_map<uint32_t, BluePrintStaticObjectSprite> blueprints_static_object_sprite;
static std::unordered_map<uint32_t, BluePrintStaticObjectAnimation> blueprints_static_object_animation;

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
		mylib_assert_exception_msg(success, "failed to insert blueprint for Tree_00");
	}

	// castle_00
	{
		const auto [it, success] = blueprints_static_object_sprite.insert({
			std::to_underlying(Object::Subtype::Castle_00),
			{
				.subtype = Object::Subtype::Castle_00,
				.texture = Texture::castle_00,
				.collider_size = Vector3(5, 5, 5),
				.sprite_size = Vector2(5.5, 5.5),
				.sprite_source_anchor = Vector2(0, -0.5),
				.sprite_dest_anchor = Vector3(-1.7, -1.7, -2.5)
			}
		});
		mylib_assert_exception_msg(success, "failed to insert blueprint for Castle_00");
	}

	// explosion
	{
		const auto [it, success] = blueprints_static_object_animation.insert({
			std::to_underlying(Object::Subtype::Explosion),
			{
				.subtype = Object::Subtype::Explosion,
				.textures = Texture::matrix_explosion.to_span(),
				.collider_size = Vector3(0, 0, 0), // no collider
				.sprite_size = Vector2(2, 2),
				.sprite_source_anchor = Vector2(0, 0),
				.sprite_dest_anchor = Vector3(0, 0, 0),
				.frame_duration = 0.05f,
				.die_after_animation = true
			}
		});
		mylib_assert_exception_msg(success, "failed to insert blueprint for Explosion");
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

	auto r = std::make_unique<StaticObjectSprite>(
		world,
		blueprint.subtype,
		pos,
		blueprint.texture,
		blueprint.sprite_size,
		blueprint.sprite_source_anchor,
		blueprint.sprite_dest_anchor
	);

	if (blueprint.collider_size != Vector3::zero())
		r->get_colliders().push_back(Collider {
			.object = r.get(),
			.ds = Vector::zero(),
			.size = blueprint.collider_size,
			.id = 0
		});

	return r;
}

// ---------------------------------------------------

std::unique_ptr<StaticObjectAnimation> build_static_object_animation (
	World *world,
	const Object::Subtype subtype,
	const Vector& pos
	)
{
	const auto it = blueprints_static_object_animation.find({std::to_underlying(subtype)});
	mylib_assert_exception_msg(it != blueprints_static_object_animation.end(), "blueprint not found for ", subtype);

	const auto& blueprint = it->second;

	auto r = std::make_unique<StaticObjectAnimation>(
		world,
		blueprint.subtype,
		pos,
		blueprint.textures,
		blueprint.sprite_size,
		blueprint.sprite_source_anchor,
		blueprint.sprite_dest_anchor,
		blueprint.frame_duration,
		blueprint.die_after_animation
	);

	if (blueprint.collider_size != Vector3::zero())
		r->get_colliders().push_back(Collider {
			.object = r.get(),
			.ds = Vector::zero(),
			.size = blueprint.collider_size,
			.id = 0
		});

	return r;
}

// ---------------------------------------------------

void Object::render (const float dt)
{

}

void Object::update (const float dt)
{

}

void Object::collision (const Collider& my_collider, const Collider& other_collider, const Vector& ds)
{

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
	this->render_colliders(Colors::red);
#endif

	this->sprite.render();
}

// ---------------------------------------------------

void StaticObjectAnimation::render (const float dt)
{
#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__
	this->render_colliders(Colors::red);
#endif

	this->animation.render(dt);
}

// ---------------------------------------------------

StaticObjectAnimation::StaticObjectAnimation (World *world_, const Subtype subtype_, const Point& pos_, const std::span<TextureDescriptor> textures_, const Vector2& size_, const Vector2 source_anchor_, const Vector3 dest_anchor_, const float frame_duration_, const bool die_after_animation)
	: StaticObject(world_, subtype_, pos_),
		animation(this, textures_, size_, source_anchor_, dest_anchor_, frame_duration_)
{
	if (die_after_animation) {
		this->animation_event_descriptor = this->animation.get_ref_event_handler().subscribe( Mylib::Event::make_callback_lambda<FooEvent>(
				[this] (const SpriteAnimation::Event& event) {
					this->world->remove_object_next_frame(this);
				}
			));
	}
}

// ---------------------------------------------------

StaticObjectAnimation::~StaticObjectAnimation ()
{
	if (this->animation_event_descriptor.is_valid())
		this->animation.get_ref_event_handler().unsubscribe(this->animation_event_descriptor);
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

	this->event_key_down_d = event_manager->key_down().subscribe( Mylib::Event::make_callback_object<MyGlib::Event::KeyDown::Type>(*this, &PlayerObject::event_key_down_callback) );
}

// ---------------------------------------------------

PlayerObject::~PlayerObject ()
{
	event_manager->key_down().unsubscribe(this->event_key_down_d);
}

// ---------------------------------------------------

void PlayerObject::render (const float dt)
{
#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__
	this->render_colliders(Colors::red);
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

	if (!stop) {
		const Vector2 new_vel = Mylib::Math::with_length( direction_vector[ std::to_underlying(this->direction) ], speed);
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

void PlayerObject::event_key_down_callback (const MyGlib::Event::KeyDown::Type& event)
{
	switch (event.key_code)
	{
		case SDLK_SPACE:
			this->vel.z = 5;
		break;

		case SDLK_LCTRL: {
			constexpr float distance = 0.1f;
			const Vector2 direction = direction_vector[ std::to_underlying(this->direction) ];
			const Vector2 ds = Mylib::Math::with_length(direction, distance);
			const Vector pos = this->get_ref_pos() + Vector(ds);
			this->world->add_object( std::make_unique<SpellObject>(this->world, pos, Vector(direction)) );
			audio_manager->play_audio(Audio::spell);
		}
		break;
	
		default:
			break;
	}
}

// ---------------------------------------------------

EnemyObject::EnemyObject (World *world_, const std::initializer_list<Point2> positions)
	: DynamicObject(world_, Subtype::Enemy),
	  sprite(
	      this,
	      Texture::enemy_00,
	      Vector2(0.4, 0.7),
	      Vector2(0, -0.5),
	      Vector3(0, 0, -0.35)
	  )
{
	this->colliders.push_back(Collider {
		.object = this,
		.ds = Vector::zero(),
		.size = Vector(0.3, 0.3, 0.7),
		.id = 0
	});

	this->coroutine = [] (EnemyObject *enemy, const std::initializer_list<Point2> positions__) -> Coroutine {
		boost::container::static_vector<Point2, 8> positions = positions__;
		EnemyObject& self = *enemy;
		auto it_previous_pos = positions.begin();
		auto it_next_position = positions.begin();
		constexpr float speed = 1.0f;
		constexpr float delay = 1.0f;

		while (true) {
			it_next_position++;

			if (it_next_position == positions.end())
				it_next_position = positions.begin();
			
			const Point2& previous_pos = *it_previous_pos;
			const Point2& next_pos = *it_next_position;

			const float distance = Mylib::Math::distance(previous_pos, next_pos);
			const float time = distance / speed;

//			dprintln("interpolating from ", previous_pos, " to ", next_pos, " in ", time, " seconds");
			
			interpolation_manager.interpolate_linear(time, &self.get_ref_pos().x, previous_pos.x, next_pos.x);
			co_await interpolation_manager.coroutine_wait_interpolate_linear(time, &self.get_ref_pos().y, previous_pos.y, next_pos.y);
//			dprintln("interpolating done, sleeping...");
			co_await timer.coroutine_wait(float_to_ClockDuration(delay));

			it_previous_pos = it_next_position;
		}
	}(this, positions);

	Mylib::initialize_coroutine(this->coroutine);
}

// ---------------------------------------------------

EnemyObject::~EnemyObject ()
{
	timer.unregister_coroutine(this->coroutine);
	interpolation_manager.unregister_coroutine(this->coroutine);
	this->coroutine.handler.destroy();
}

// ---------------------------------------------------

void EnemyObject::render (const float dt)
{
#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__
	this->render_colliders(Colors::red);
#endif

	this->sprite.render();
}

// ---------------------------------------------------

void EnemyObject::update (const float dt)
{
}

// ---------------------------------------------------

void EnemyObject::collision (const Collider& my_collider, const Collider& other_collider, const Vector& ds)
{
	const Object *other_object = other_collider.object;

	if (other_object->get_type() == Object::Type::Spell) { // die
		auto *explosion_obj = static_cast<StaticObjectAnimation*>( this->world->add_object( build_static_object_animation(
			this->world,
			Object::Subtype::Explosion,
			this->get_value_pos()
			)));
		this->world->remove_object_next_frame(this);
		audio_manager->play_audio(Audio::enemy_death);
	}
}

// ---------------------------------------------------

SpellObject::SpellObject (World *world_, const Point& pos_, const Vector& direction_)
	: DynamicObject(world_, Subtype::Spell, pos_),
	  cube(Config::spell_size),
	  color_interpolator(Config::spell_color_time, &this->color, Colors::random(random_generator), Colors::random(random_generator)),
	  axis(random_vector<Vector3>()),
	  angle(0.0f)
{
	this->colliders.push_back(Collider {
		.object = this,
		.ds = Vector::zero(),
		.size = Vector(Config::spell_size, Config::spell_size, Config::spell_size),
		.id = 0
	});

	this->vel = Mylib::Math::with_length(direction_, Config::spell_speed);

	this->timer_descriptor = timer.schedule_event(Clock::now() + float_to_ClockDuration(Config::spell_life_span), Mylib::Event::make_callback_lambda<Timer::Event>(
		[this] (const Timer::Event& event) {
			this->world->remove_object_next_frame(this);
		}));
}

// ---------------------------------------------------

SpellObject::~SpellObject ()
{
	if (this->timer_descriptor.is_valid()) {
		timer.unschedule_event(this->timer_descriptor);
	}
}

// ---------------------------------------------------

void SpellObject::render (const float dt)
{
#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__
	this->render_colliders(Colors::red);
#endif

	this->angle = std::fmod(this->angle + Config::spell_angular_speed * dt, Mylib::Math::degrees_to_radians(fp(360)));
	this->cube.rotate(this->axis, this->angle);

	if (!this->color_interpolator(dt))
		Mylib::reconstruct(this->color_interpolator, Config::spell_color_time, &this->color, this->color, Colors::random(random_generator));

	renderer->draw_cube3D(this->cube, this->get_ref_pos(), this->color);
}

// ---------------------------------------------------

void SpellObject::update (const float dt)
{
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