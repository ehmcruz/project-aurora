#include <thread>

#include <my-game-lib/my-game-lib.h>
#include <my-game-lib/debug.h>

#include <my-lib/math.h>

#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>

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

Point camera_pos(-0.5, -0.5, -10);
Vector camera_vector(0, 0, 1);
Color ambient_light_color {1, 1, 1, 0.8};
Cube3D far_cube (5);
TextureDescriptor texture;

void render ()
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

	const Vector far_cube_pos = Vector(0, 0, 50);
	const Color far_cube_color = Color::yellow();

	renderer->draw_cube3D(far_cube, far_cube_pos, far_cube_color);

	renderer->draw_rect2D(Rect2D(3, 2), Vector(0, 0, 1), TextureRenderOptions { .desc = texture });

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

	renderer->begin_texture_loading();
	texture = renderer->load_texture("assets/main-char-walk/GreatSwordKnight_2hWalk_dir1.png");
	renderer->end_texture_loading();

	constexpr float dt = 1.0 / 30.0;
	uint64_t frame = 0;

	while (alive)
	{
		std::cout << "rendering frame " << frame << std::endl;

		event_manager->process_events();

		render();

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