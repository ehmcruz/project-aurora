#include <thread>

#include <my-game-lib/my-game-lib.h>
#include <my-game-lib/debug.h>

#include <my-lib/math.h>

#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/graphics.h>
#include <aurora/audio.h>
#include <aurora/object.h>
#include <aurora/world.h>
#include <aurora/main.h>

// ---------------------------------------------------

namespace Game
{

// ---------------------------------------------------

Main::Main (const InitConfig& cfg)
{
	this->state = State::Initializing;
	this->cfg_params = cfg;

	game_lib = &MyGlib::Lib::init({
		.graphics_type = MyGlib::Graphics::Manager::Type::Opengl,
		.window_name = "Project Aurora",
		.window_width_px = cfg.window_width_px,
		.window_height_px = cfg.window_height_px,
		.fullscreen = cfg.fullscreen
	});

	event_manager = &game_lib->get_event_manager();
	audio_manager = &game_lib->get_audio_manager();
	renderer = &game_lib->get_graphics_manager();

	std::random_device rd;
	random_generator.seed( rd() );

	load_graphics();
	load_audio();
	load_objects();

	dprintln("chorono resolution ", (static_cast<float>(Clock::period::num) / static_cast<float>(Clock::period::den)));

	this->world = nullptr;
	this->world = new World();

	dprintln("loaded world");

	this->alive = true;

	this->event_quit_d = event_manager->quit().subscribe( Mylib::Event::make_callback_object<MyGlib::Event::Quit::Type>(*this, &Main::event_quit) );
	this->event_key_down_d = event_manager->key_down().subscribe( Mylib::Event::make_callback_object<MyGlib::Event::KeyDown::Type>(*this, &Main::event_key_down_callback) );
}

// ---------------------------------------------------

Main::~Main ()
{
	delete this->world;
	event_manager->quit().unsubscribe(this->event_quit_d);
	event_manager->key_down().unsubscribe(this->event_key_down_d);
	MyGlib::Lib::quit();
}

// ---------------------------------------------------

Game::Main* Main::load (const InitConfig& cfg)
{
	instance = new Main(cfg);
	return instance;
}

// ---------------------------------------------------

void Main::unload ()
{
	delete instance;
	instance = nullptr;
}

// ---------------------------------------------------

void Main::event_quit (const MyGlib::Event::Quit::Type)
{
	this->alive = false;
}

// ---------------------------------------------------

void Main::event_key_down_callback (const MyGlib::Event::KeyDown::Type& event)
{
	switch (event.key_code)
	{
		case SDLK_ESCAPE:
			this->alive = false;
		break;
	
		default:
			break;
	}
}

// ---------------------------------------------------

void Main::run ()
{
	float real_dt, virtual_dt, required_dt, sleep_dt, busy_wait_dt, fps;

	this->state = State::Playing;

	real_dt = 0.0f;
	virtual_dt = 0.0f;
	required_dt = 0.0f;
	sleep_dt = 0.0f;
	busy_wait_dt = 0.0f;
	fps = 0.0f;

	while (this->alive) {
		const ClockTime tbegin = Clock::now();
		ClockTime tend;
		ClockDuration elapsed;

		renderer->wait_next_frame();

		virtual_dt = (real_dt > Config::max_dt) ? Config::max_dt : real_dt;

		timer.trigger_events();
		interpolation_manager.process_interpolation(virtual_dt);

	#if 0
		dprintln("start new frame render target_dt=", Config::target_dt,
			" required_dt=", required_dt,
			" real_dt=", real_dt,
			" sleep_dt=", sleep_dt,
			" busy_wait_dt=", busy_wait_dt,
			" virtual_dt=", virtual_dt,
			" max_dt=", Config::max_dt,
			" target_dt=", Config::target_dt,
			" fps=", fps
			);
	#endif

		event_manager->process_events();

		switch (this->state) {
			case State::Playing:
				this->world->process_update(virtual_dt);
				this->world->process_physics(virtual_dt);
				this->world->render(virtual_dt);
			break;
			
			default:
				mylib_assert_exception(0)
		}

		renderer->render();
		renderer->update_screen();

		switch (this->state) {
			case State::Playing:
				this->world->frame_finished();
			break;
			
			default:
				mylib_assert_exception(0)
		}

		const ClockTime trequired = Clock::now();
		elapsed = trequired - tbegin;
		required_dt = ClockDuration_to_float(elapsed);

		if constexpr (Config::sleep_to_save_cpu) {
			if (required_dt < Config::sleep_threshold) {
				sleep_dt = Config::sleep_threshold - required_dt; // target sleep time
				std::this_thread::sleep_for(float_to_ClockDuration(sleep_dt));
			}
		}
		
		const ClockTime tbefore_busy_wait = Clock::now();
		elapsed = tbefore_busy_wait - trequired;
		sleep_dt = ClockDuration_to_float(elapsed); // check exactly time sleeping

		do {
			tend = Clock::now();
			elapsed = tend - tbegin;
			real_dt = ClockDuration_to_float(elapsed);

			if constexpr (!Config::busy_wait_to_ensure_fps)
				break;
		} while (real_dt < Config::target_dt);

		elapsed = tend - tbefore_busy_wait;
		busy_wait_dt = ClockDuration_to_float(elapsed);

		fps = 1.0f / real_dt;
	}
}

// ---------------------------------------------------

} // end namespace Game

// ---------------------------------------------------

int main (int argc, char **argv)
{
	try {
		Game::Main *game = Game::Main::load({
			.window_width_px = 1920,
			.window_height_px = 1080,
			.fullscreen = false
		});
		
		game->run();

		Game::Main::unload();
	}
	catch (const std::exception& e) {
		Game::dprintln("Something bad happened!", '\n', e.what());
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}