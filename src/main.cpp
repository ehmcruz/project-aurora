#include <thread>

#include <my-game-lib/my-game-lib.h>
#include <my-game-lib/debug.h>

#include <my-lib/math.h>

#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/graphics.h>
#include <aurora/object.h>

// ---------------------------------------------------

namespace Game
{

// ---------------------------------------------------

bool alive = true;

// ---------------------------------------------------

void quit_callback (const MyGlib::Event::Quit::Type& event)
{
	alive = false;
}

// ---------------------------------------------------

LightPointDescriptor light;
Point camera_pos(0, 0, 10);
Vector camera_vector(0, 0, -1);
Color ambient_light_color {1, 1, 1, 0.8};
Cube3D far_cube (10);
Mylib::Matrix<TextureDescriptor> textures;
ObjectSpriteAnimation *main_char;
ObjectSprite *tree;

void render (const float dt)
{
	renderer->wait_next_frame();

	renderer->setup_render_3D( MyGlib::Graphics::RenderArgs3D {
		.world_camera_pos = camera_pos,
		.world_camera_target = camera_pos + camera_vector,
		.fov_y = Mylib::Math::degrees_to_radians(fp(45)),
		.z_near = 0.1,
		.z_far = 100,
		.ambient_light_color = ambient_light_color,
		} );

	const Vector far_cube_pos = Vector(0, 0, -50);
	const Color far_cube_color = Color::yellow();

	renderer->draw_cube3D(far_cube, far_cube_pos, far_cube_color);

	//tree->render(dt);
	main_char->render(dt);
	tree->render(dt);

	renderer->render();
	renderer->update_screen();
}

// ---------------------------------------------------

int main (const int argc, const char **argv)
{
	dprintln("Initializing...");

	game_lib = &MyGlib::Lib::init({
		.graphics_type = MyGlib::Graphics::Manager::Type::Opengl,
		.window_name = "Project Aurora",
		.window_width_px = 1200,
		.window_height_px = 800,
		//.fullscreen = true
		.fullscreen = false
	});
	event_manager = &game_lib->get_event_manager();
	audio_manager = &game_lib->get_audio_manager();
	renderer = &game_lib->get_graphics_manager();

	event_manager->quit().subscribe( Mylib::Trigger::make_callback_function<MyGlib::Event::Quit::Type>(&quit_callback) );

	dprintln("SDL initialized!");

	load_textures();

	textures = renderer->split_texture(Texture::main_char, 3, 3);

	main_char = new ObjectSpriteAnimation(nullptr, Rect2D(3, 4), textures.to_span(), 0.1);
	main_char->set_pos(Vector(0, 0, -1));
	main_char->get_ref_sprite_animation().set_scale(3);

	tree = new ObjectSprite(nullptr, Rect2D(5, 5), Texture::tree);
	tree->set_pos(Vector(2, 1, 0));

	light = renderer->add_light_point_source(
		Point(0, 0, 10), Color::white()
	);

	constexpr float dt = 1.0 / 30.0;
	uint64_t frame = 0;

	while (alive)
	{
		std::cout << "rendering frame " << frame << std::endl;

		event_manager->process_events();

		render(dt);

		std::this_thread::sleep_for(float_to_ClockDuration(dt));
		frame++;
	}

	return 0;
}

// ---------------------------------------------------

} // end namespace Game

// ---------------------------------------------------

int main (int argc, char **argv)
{
	try {
		Game::main(argc, const_cast<const char**>(argv));
	}
	catch (const std::exception& e) {
		Game::dprintln("Something bad happened!", '\n', e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}