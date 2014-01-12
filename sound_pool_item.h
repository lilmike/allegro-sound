//sound_pool_item.h
//By Michael Taboada
//Provided under the unlicense
//see UNLICENSE for details
#define _USE_MATH_DEFINES
#include <math.h>
#include <string>
#include "sound.h"

#pragma once
class sound_pool_item
{
public:
	sound_pool_item(void);
	~sound_pool_item(void);
	void reset();
	void update(int listener_x, int listener_y, int listener_z, double listener_angle, int max_distance);
	void update_listener_position(int listener_x, int listener_y, int listener_z, double listener_angle);
	int get_total_distance(int listener_x, int listener_y, int listener_z);
	sound* s;
	string filename;
	int x;
	int y;
	int z;
	bool looping;
	float pan_step;
	float volume_step;
	float behind_pitch_decrease;
	float behind_volume_decrease;
	float above_pitch;
	float start_pan;
	float start_volume;
	float start_pitch;
	int left_range;
	int right_range;
	int forward_range;
	int backward_range;
	int up_range;
	int down_range;
	bool is_2d;
	bool is_3d;
	bool paused;
	bool stationary;
	bool persistent;
	void position_sound_1d(sound* s, int x, int sx, double pan_step = 1.0, double volume_step = 1.0, double start_pan = 0, double start_volume = 0, double start_pitch = 100);
	void position_sound_2d(sound* s, int x, int y, double angle, int sx, int sy, double pan_step = 1.0, double volume_step = 1.0, double behind_pitch_decrease = 0.5, double behind_volume_decrease = 5.0, double start_pan = 0, double start_volume = 0, double start_pitch = 100);
	void position_sound_3d(sound* s, double x, double y, double z, double theta, double sx, double sy, double sz, double pan_step = 1.0, double volume_step = 1.0, double above_pitch = 5.0, double behind_pitch_decrease = 0.5, double behind_volume_decrease = 5.0, double start_pan = 0, double start_volume = 0, double start_pitch = 100);
};

