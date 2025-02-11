#ifndef __PROJECT_AURORA_TYPES_HEADER_H__
#define __PROJECT_AURORA_TYPES_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <chrono>

#include <SDL.h>

#include <my-lib/std.h>
#include <my-lib/macros.h>
#include <my-lib/math-vector.h>

#include <my-game-lib/my-game-lib.h>
#include <my-game-lib/opengl/opengl.h>


namespace Game
{

// ---------------------------------------------------

using MyGlib::Graphics::Vector;
using MyGlib::Graphics::Point;
using MyGlib::Graphics::Quaternion;
using MyGlib::Graphics::Vector2;
using MyGlib::Graphics::Vector4f;
using MyGlib::Graphics::Color;
using MyGlib::Graphics::LightPointDescriptor;
using MyGlib::Graphics::Cube3D;
using MyGlib::Graphics::Sphere3D;
using MyGlib::Graphics::Rect2D;
using MyGlib::Graphics::Circle2D;
using MyGlib::Graphics::Line3D;
using MyGlib::Graphics::WireCube3D;
using MyGlib::Graphics::TextureDescriptor;
using MyGlib::Graphics::TextureRenderOptions;
using MyGlib::Graphics::Opengl::Opengl_TextureDescriptor;

using GraphicsVertex = MyGlib::Graphics::Opengl::ProgramTriangleTexture::Vertex;

using VectorBasis = Mylib::Math::VectorBasis3<float>;

using Clock = std::chrono::steady_clock;
using ClockDuration = Clock::duration;
using ClockTime = Clock::time_point;

// ---------------------------------------------------

} // end namespace Game

#endif