#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/graphics.h>
#include <aurora/object.h>
#include <aurora/world.h>


namespace Game
{

// ---------------------------------------------------

void Object::physics (const float dt)
{
	this->pos += this->vel * dt;
}

// ---------------------------------------------------

void ObjectSprite::render (const float dt)
{
	this->sprite.render();
}

// ---------------------------------------------------

void ObjectSpriteAnimation::render (const float dt)
{
	this->sprite_animation.render(dt);
}

// ---------------------------------------------------

} // end namespace Game