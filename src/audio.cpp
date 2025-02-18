#include <aurora/config.h>
#include <aurora/types.h>
#include <aurora/lib.h>
#include <aurora/globals.h>
#include <aurora/audio.h>
#include <aurora/object.h>


namespace Game
{

// ---------------------------------------------------

void load_audio ()
{
	using namespace Audio;

	spell = audio_manager->load_sound("assets/magical-spell-cast-190272.wav", MyGlib::Audio::Format::Wav);
	enemy_death = audio_manager->load_sound("assets/male-death-sound-128357.wav", MyGlib::Audio::Format::Wav);
	background_music = audio_manager->load_music("assets/medieval-background-196571.mp3", MyGlib::Audio::Format::MP3);
}

// ---------------------------------------------------

} // end namespace Game