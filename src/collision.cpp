#include <cmath>

#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/graphics.h>
#include <aurora/object.h>
#include <aurora/collision.h>
#include <aurora/world.h>


namespace Game
{

// ---------------------------------------------------

std::pair<bool, Vector> Collider::check_collision (const Collider& a, const Collider& b)
{
	Vector collision_vector;

	const Vector a_pos = a.object->get_ref_pos() + a.ds;
	const Vector b_pos = b.object->get_ref_pos() + b.ds;

	const Vector distance = b_pos - a_pos;
	const Vector target_distance = a.half_size + b.half_size;

	/*
		Let's think a bit.
		First, we only care about a valid collision_vector
		if the colliders are colliding.
		If they are not colliding, we don't care if the collision_vector
		calculated is valid or not.
	*/

	/*
		x axis is in the right direction in the plane.
		If b is right of a, then the distance.x is positive.
		Since the collider returns what we should do with object b
		to solve the collision, collision_vector.x should be positive.
		Otherwise, it should be negative.
	*/

	const bool colliding_x = std::abs(distance.x) < target_distance.x;
	collision_vector.x = std::copysign(target_distance.x - std::abs(distance.x), distance.x);

	/*
		y axis is in the up direction in the plane.
		If b is above a, then the distance.y is positive.
		Since the collider returns what we should do with object b
		to solve the collision, collision_vector.y should be positive.
		Otherwise, it should be negative.
	*/

	const bool colliding_y = std::abs(distance.y) < target_distance.y;
	collision_vector.y = std::copysign(target_distance.y - std::abs(distance.y), distance.y);

	/*
		z axis represents the altitude.
		The higher the altitude, the higher the z value, since we use right-handed coordinates.
		If b is above a, then the distance.y is positive.
		Since the collider returns what we should do with object b
		to solve the collision, collision_vector.y should be positive.
		Otherwise, it should be negative.
	*/

	const bool colliding_z = std::abs(distance.z) < target_distance.z;
	collision_vector.z = std::copysign(target_distance.z - std::abs(distance.z), distance.z);

	return std::pair<bool, Vector>(colliding_x && colliding_y && colliding_z, collision_vector);
}

// ---------------------------------------------------

#ifdef AURORA_DEBUG_ENABLE_RENDER_COLLIDERS__

void Collider::render (const Color& color) const
{
	auto cube = WireCube3D(this->half_size * fp(2));

	renderer->draw_wire_cube3D(cube, this->object->get_ref_pos() + this->ds, color);
}

#endif

// ---------------------------------------------------

} // end namespace Game