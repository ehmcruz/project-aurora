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

void Object::render_colliders (const Color& color) const
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
	this->colliders.push_back(Collider(this, Vector::zero(), Vector(1, 1, 1), 0));
}

// ---------------------------------------------------

void PlayerObject::render (const float dt)
{
#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__
	this->render_colliders(Color::red());
#endif
}

// ---------------------------------------------------

void PlayerObject::update (const float dt)
{
}

// ---------------------------------------------------

} // end namespace Game