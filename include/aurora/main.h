#ifndef __PROJECT_AURORA_MAIN_HEADER_H__
#define __PROJECT_AURORA_MAIN_HEADER_H__

#ifdef __MINGW32__
	#define SDL_MAIN_HANDLED
#endif

#include <my-game-lib/my-game-lib.h>


namespace Game
{

// ---------------------------------------------------

class World;

// ---------------------------------------------------

class Main
{
public:
	struct InitConfig {
		uint32_t window_width_px;
		uint32_t window_height_px;
		bool fullscreen;
	};

	enum class State {
		Initializing,
		Playing
	};

protected:
	MYLIB_OO_ENCAPSULATE_PTR(World*, world)
	MYLIB_OO_ENCAPSULATE_SCALAR(bool, alive)
	MYLIB_OO_ENCAPSULATE_SCALAR_READONLY(State, state)
	MYLIB_OO_ENCAPSULATE_SCALAR_READONLY(InitConfig, cfg_params)

	MyGlib::Event::Quit::Descriptor event_quit_d;
	MyGlib::Event::KeyDown::Descriptor event_key_down_d;

private:
	static inline Main *instance = nullptr;

	Main (const InitConfig& cfg);
	~Main ();

public:
	void run ();
	void event_quit (const MyGlib::Event::Quit::Type);
	void event_key_down_callback (const MyGlib::Event::KeyDown::Type& event);

	static inline Main& get () noexcept
	{
		return *instance;
	}

	static Main* load (const InitConfig& cfg);
	static void unload ();
};

// ---------------------------------------------------

} // end namespace Game

#endif