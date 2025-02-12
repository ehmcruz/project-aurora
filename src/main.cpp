#include <thread>

#include <my-game-lib/my-game-lib.h>
#include <my-game-lib/debug.h>

#include <my-lib/math.h>

#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/graphics.h>
#include <aurora/object.h>
#include <aurora/world.h>

// ---------------------------------------------------

namespace Game
{

// ---------------------------------------------------

bool alive = true;
World *world;

// ---------------------------------------------------

void quit_callback (const MyGlib::Event::Quit::Type& event)
{
	alive = false;
}

// ---------------------------------------------------

//ObjectSprite *tree;

void render (const float dt)
{
	renderer->wait_next_frame();

	world->render(dt);

	//tree->render(dt);

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

	load_graphics();
	load_objects();

	//tree = new ObjectSprite(nullptr, Rect2D(5, 5), Texture::grass);
	//tree->set_pos(Vector(2, 2, 0));

	world = new World();

	constexpr float dt = 1.0 / 30.0;
	uint64_t frame = 0;

	while (alive)
	{
		std::cout << "rendering frame " << frame << std::endl;

		event_manager->process_events();

		world->process_update(dt);
		world->process_physics(dt);
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