//sound.h
//By Michael Taboada
//Provided under the unlicense
//see UNLICENSE for details
#include <stdio.h>
#include <map>
#include <string>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

using std::string;
using std::map;
using std::pair;

#pragma once
class sound
{
public:
	sound(void);
	~sound(void);
	bool load(string file, string mixer = "default");
	double get_pan();
	bool set_pan(double pan_value);
	double get_gain();
	bool set_gain(double gain);
	double get_pitch();
	bool set_pitch(double pitch);
	bool get_loop();
	bool set_loop(bool loop);
	bool play();
	bool stop();
	bool is_playing();
	bool pause();
	bool get_paused();
	bool get_active();
private:
	double pan(double doubleval);
	double dB2lin(double dB);
	bool looping;
	double panval;
	double gainval;
	double pitchval;
	bool paused;
	bool active;
	ALLEGRO_SAMPLE_INSTANCE* si;
	static map<string, ALLEGRO_SAMPLE*> samples;
	static map<string, ALLEGRO_MIXER*> mixers;
	static ALLEGRO_VOICE* voice;
	void log(string s);
	static int sounds;
};

