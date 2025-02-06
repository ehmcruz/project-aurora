#include <thread>

#include <my-game-lib/my-game-lib.h>
#include <my-game-lib/debug.h>

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
	TextureDescriptor texture = renderer->load_texture("assets/main-char-walk/GreatSwordKnight_2hWalk_dir1.png");
	renderer->end_texture_loading();

	constexpr float dt = 1.0 / 60.0;

	while (alive)
	{
		//std::cout << "rendering frame " << frame << std::endl;

		event_manager->process_events();

		std::this_thread::sleep_for(float_to_ClockDuration(dt));
//		frame++;
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