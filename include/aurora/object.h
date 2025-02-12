#ifndef __PROJECT_AURORA_OBJECT_HEADER_H__
#define __PROJECT_AURORA_OBJECT_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <SDL.h>

#include <string>
#include <list>
#include <memory>

#include <my-lib/std.h>
#include <my-lib/macros.h>

#include <my-game-lib/my-game-lib.h>

#include <aurora/config.h>
#include <aurora/types.h>
#include <aurora/graphics.h>
#include <aurora/collision.h>


namespace Game
{

// ---------------------------------------------------

class World;

// ---------------------------------------------------

class Object
{
public:
	#define _MYLIB_ENUM_CLASS_OBJECT_TYPE_VALUES_ \
		_MYLIB_ENUM_CLASS_OBJECT_TYPE_VALUE_(Map) \
		_MYLIB_ENUM_CLASS_OBJECT_TYPE_VALUE_(Tree) \
		_MYLIB_ENUM_CLASS_OBJECT_TYPE_VALUE_(Castle) \
		_MYLIB_ENUM_CLASS_OBJECT_TYPE_VALUE_(Character)

	enum class Type : uint32_t {
		#define _MYLIB_ENUM_CLASS_OBJECT_TYPE_VALUE_(V) V,
		_MYLIB_ENUM_CLASS_OBJECT_TYPE_VALUES_
		#undef _MYLIB_ENUM_CLASS_OBJECT_TYPE_VALUE_
	};

	// when add subtypes, first we specify the type, and then the subtype

	#define _MYLIB_ENUM_CLASS_OBJECT_SUBTYPE_VALUES_ \
		_MYLIB_ENUM_CLASS_OBJECT_SUBTYPE_VALUE_(Map, Map) \
		_MYLIB_ENUM_CLASS_OBJECT_SUBTYPE_VALUE_(Tree, Tree_00) \
		_MYLIB_ENUM_CLASS_OBJECT_SUBTYPE_VALUE_(Castle, Castle_00) \
		_MYLIB_ENUM_CLASS_OBJECT_SUBTYPE_VALUE_(Character, Player)

	enum class Subtype : uint32_t {
		#define _MYLIB_ENUM_CLASS_OBJECT_SUBTYPE_VALUE_(TYPE, V) V,
		_MYLIB_ENUM_CLASS_OBJECT_SUBTYPE_VALUES_
		#undef _MYLIB_ENUM_CLASS_OBJECT_SUBTYPE_VALUE_
	};

private:
	inline static constexpr Type types__[] = {
		#define _MYLIB_ENUM_CLASS_OBJECT_SUBTYPE_VALUE_(TYPE, V) Type::TYPE,
		_MYLIB_ENUM_CLASS_OBJECT_SUBTYPE_VALUES_
		#undef _MYLIB_ENUM_CLASS_OBJECT_SUBTYPE_VALUE_
	};

public:
	constexpr static Type get_type (const Subtype subtype) noexcept
	{
		return types__[ std::to_underlying(subtype) ];
	}

protected:
	MYLIB_OO_ENCAPSULATE_PTR(World*, world)
	MYLIB_OO_ENCAPSULATE_SCALAR_READONLY(Type, type)
	MYLIB_OO_ENCAPSULATE_SCALAR_READONLY(Subtype, subtype)
	MYLIB_OO_ENCAPSULATE_OBJ_WITH_COPY_MOVE(std::string, name)

public:
	inline Object (World *world_, const Type type_, const Subtype subtype_)
		: world(world_), type(type_), subtype(subtype_)
	{
	}

	virtual ~Object () = default;

	virtual void render (const float dt) = 0;
	virtual void update (const float dt) = 0;
};

// ---------------------------------------------------

class StaticObject : public Object
{
	MYLIB_OO_ENCAPSULATE_OBJ_INIT_WITH_COPY_MOVE(Point, pos, Point::zero())

protected:
	std::list<Collider> colliders;

public:
	inline StaticObject (World *world_, const Type type_, const Subtype subtype_)
		: Object(world_, type_, subtype_)
	{
	}

	inline StaticObject (World *world_, const Type type_, const Subtype subtype_, const Point& pos_)
		: Object(world_, type_, subtype_), pos(pos_)
	{
	}

	inline auto& get_colliders () noexcept
	{
		return this->colliders;
	}

#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__
	void render_colliders (const Color& color) const;
#endif
};

// ---------------------------------------------------

class StaticObjectSprite : public StaticObject
{
	MYLIB_OO_ENCAPSULATE_OBJ(Sprite, sprite)

public:
	StaticObjectSprite (World *world_, const Type type_, const Subtype subtype_, const Point& pos_, const TextureDescriptor& texture_, const Vector2& size_, const Vector2& ds_)
		: StaticObject(world_, type_, subtype_, pos_), sprite(this, texture_, size_, ds_)
	{
	}

	void render (const float dt) override final;
	void update (const float dt) override final;
};

// ---------------------------------------------------

class DynamicObject : public StaticObject
{
protected:
	MYLIB_OO_ENCAPSULATE_OBJ_INIT_WITH_COPY_MOVE(Vector, vel, Vector::zero())

public:
	inline DynamicObject (World *world_, const Type type_, const Subtype subtype_)
		: StaticObject(world_, type_, subtype_)
	{
	}

	inline DynamicObject (World *world_, const Type type_, const Subtype subtype_, const Point& pos_)
		: StaticObject(world_, type_, subtype_, pos_)
	{
	}
	
	inline void physics (const float dt) noexcept
	{
		this->pos += this->vel * dt + Config::gravity * dt * dt / fp(2);
		this->vel += Config::gravity * dt;
	}
};

// ---------------------------------------------------

class PlayerObject : public DynamicObject
{
private:
	Sprite sprite;

public:
	PlayerObject (World *world_, const Point& pos_);

	void render (const float dt) override final;
	void update (const float dt) override final;
};

// ---------------------------------------------------

void load_objects ();

std::unique_ptr<StaticObjectSprite> build_static_object_sprite (
	World *world,
	const Object::Subtype subtype,
	const Vector& pos
	);

// ---------------------------------------------------

const char* enum_class_to_str (const Object::Type value);
const char* enum_class_to_str (const Object::Subtype value);

// ---------------------------------------------------

inline std::ostream& operator << (std::ostream& out, const Object::Type value)
{
	out << enum_class_to_str(value);
	return out;
}

inline std::ostream& operator << (std::ostream& out, const Object::Subtype value)
{
	out << enum_class_to_str(value);
	return out;
}

// ---------------------------------------------------

} // end namespace Game

#endif