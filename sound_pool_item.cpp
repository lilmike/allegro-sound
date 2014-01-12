//sound_pool_item.cpp
//By Michael Taboada
//Provided under the unlicense
//see UNLICENSE for details
#include "sound_pool_item.h"


sound_pool_item::sound_pool_item(void)
{
	s = NULL;
reset();
}


sound_pool_item::~sound_pool_item(void)
{
	if(s) {
		delete s;
	}
}

void sound_pool_item::reset() {
	if(s) {
		delete s;
		s = NULL;	
	}
	filename="";
	x=0;
	y=0;
	z=0;
	looping=false;
	pan_step=0.0;
	volume_step=0.0;
	behind_pitch_decrease=0.0;
	start_pan=0.0;
	start_volume=0.0;
	start_pitch=100.0;
	left_range=0;
	right_range=0;
	forward_range=0;
	backward_range=0;
	up_range=0;
	down_range=0;
	is_2d=false;
	is_3d=false;
	paused=false;
	stationary=false;
	persistent=false;
}

void sound_pool_item::update(int listener_x, int listener_y, int listener_z, double listener_angle, int max_distance) {
	if(!s) {
		return;
	}
	if(max_distance > 0 && looping) {
		int total_distance = get_total_distance(listener_x, listener_y, listener_z);
		if(total_distance>max_distance && (s->is_playing() || s->get_paused())) {
			delete s;
			s = new sound();
			return;
		}
		if(total_distance<=max_distance && !(s->is_playing() || s->get_paused())) {
			if(s->load(filename)) {
				update_listener_position(listener_x, listener_y, listener_z, listener_angle);
				if(!paused) {
					s->set_loop(true);
					s->play();
				}
			}
			return;
		}
	}
	update_listener_position(listener_x, listener_y, listener_z, listener_angle);
}

void sound_pool_item::update_listener_position(int listener_x, int listener_y, int listener_z, double listener_angle) {
	if(s) {
		if(!s->get_active()) {
			return;
		}
		if(stationary) {
			return;
		}
		int delta_left=x-left_range;
		int delta_right=x+right_range;
		int delta_backward=y-backward_range;
		int delta_forward=y+forward_range;
		int delta_up = z+up_range;
		int delta_down = z-down_range;
		int true_x=listener_x;
		int true_y=listener_y;
		int true_z = listener_z;
		if(!is_2d) {
			if(listener_x>=delta_left && listener_x<=delta_right) {
				position_sound_1d(s, listener_x, listener_x, pan_step, volume_step, start_pan, start_volume);
				return;
			}
			if(listener_x<delta_left)
				position_sound_1d(s, listener_x, delta_left, pan_step, volume_step, start_pan, start_volume);
			if(listener_x>delta_right)
				position_sound_1d(s, listener_x, delta_right, pan_step, volume_step, start_pan, start_volume);
			return;
		}
		else if(!is_3d) {
			if(listener_x < delta_left)
				true_x = delta_left;
			else if(listener_x > delta_right)
				true_x = delta_right;
			if(listener_y < delta_backward)
				true_y = delta_backward;
			else if(listener_y > delta_forward)
				true_y = delta_forward;
			position_sound_2d(s, listener_x, listener_y, listener_angle, true_x, true_y, pan_step, volume_step, behind_pitch_decrease, behind_volume_decrease, start_pan, start_volume, start_pitch);
			return;
		}
		if(listener_x < delta_left)
			true_x = delta_left;
		else if(listener_x > delta_right)
			true_x = delta_right;
		if(listener_y < delta_backward)
			true_y = delta_backward;
		else if(listener_y > delta_forward)
			true_y = delta_forward;
		if(listener_z < delta_down)
			true_z = delta_down;
		else if(listener_z > delta_up)
			true_z = delta_up;
		position_sound_3d(s, listener_x, listener_y, listener_z, listener_angle, true_x, true_y, true_z, pan_step, volume_step, above_pitch, behind_pitch_decrease, behind_volume_decrease, start_pan, start_volume, start_pitch);
	}
}

void sound_pool_item::position_sound_1d(sound* s, int x, int sx, double pan_step, double volume_step, double start_pan, double start_volume, double start_pitch) {
	position_sound_2d(s, x, 0, 0, sx, 0, pan_step, volume_step, 0, 0, start_pan, start_volume, start_pitch);
}

void sound_pool_item::position_sound_2d(sound* s, int x, int y, double angle, int sx, int sy, double pan_step, double volume_step, double behind_pitch_decrease, double behind_volume_decrease, double start_pan, double start_volume, double start_pitch) {
	position_sound_3d(s, x, y, 0, angle, sx, sy, 0, pan_step, volume_step, 0, behind_pitch_decrease, behind_volume_decrease, start_pan, start_volume, start_pitch);
}

void sound_pool_item::position_sound_3d(sound* s, double x, double y, double z, double theta, double sx, double sy, double sz, double pan_step, double volume_step, double above_pitch, double behind_pitch_decrease, double behind_volume_decrease, double start_pan, double start_volume, double start_pitch) {
	double xd = x-sx;
	double yd = y-sy;
	double zd = z-sz;
	double d = sqrt((xd*xd)+(yd*yd)+(zd*zd));
	double atheta, rtheta;
	if(yd == 0){
		if(xd == 0) {
			atheta=0;
		}
		else if(xd < 0) {
			atheta = M_PI/2;
		}
		else {
			atheta = (3*M_PI)/2;
		}
	}
	else if(yd < 0)
		atheta = atan(xd/yd);
	else
		atheta = atan(xd/yd)+M_PI;
	atheta = atheta*180.0/M_PI;
	rtheta = atheta-theta;
	double pan = sin(rtheta*M_PI/180.0);
	pan = pan*d*pan_step+start_pan;
	double behind = -cos(rtheta*M_PI/180.0);
	double volume = -d*volume_step+start_volume;
	double pitch = start_pitch;
	if(behind > 0) {
		volume = volume-behind_volume_decrease;
		pitch = pitch-behind_pitch_decrease;
	}
	double phi;
	if(zd == 0)
		phi = 0;
	else
		if(d != 0) 
			phi = atan(zd/d);
		else
			phi = M_PI/2.0*abs(zd)/zd;
	phi = phi*2.0;
	double temppitch = -above_pitch*sin(phi);
	pitch = pitch+temppitch;
	s->set_pan(pan);
	s->set_gain(volume);
	s->set_pitch(pitch);
}

int sound_pool_item::get_total_distance(int listener_x, int listener_y, int listener_z) {
	if(stationary==true) {
		return 0;
	}
	int delta_left=x-left_range;
	int delta_right=x+right_range;
	int delta_backward=y-backward_range;
	int delta_forward=y+forward_range;
	int delta_up = z+up_range;
	int delta_down = z-down_range;
	int true_x=listener_x;
	int true_y=listener_y;
	int true_z = listener_z;
	int distance=0;
	if(is_2d==false) {
		if(listener_x>=delta_left && listener_x<=delta_right) {
			return distance;
		}
		if(listener_x<delta_left)
			distance=delta_left-listener_x;
		if(listener_x>delta_right)
			distance=listener_x-delta_right;
		return distance;
	}
	else if(is_3d == false) {
		if(listener_x<delta_left)
			true_x=delta_left;
		else if(listener_x>delta_right)
			true_x=delta_right;
		if(listener_y<delta_backward)
			true_y=delta_backward;
		else if(listener_y>delta_forward)
			true_y=delta_forward;
		if(listener_x<true_x)
			distance=(true_x-listener_x);
		if(listener_x>true_x)
			distance=(listener_x-true_x);
		if(listener_y<true_y)
			distance+=(true_y-listener_y);
		if(listener_y>true_y)
			distance+=(listener_y-true_y);
		return distance;
	}
	if(listener_x<delta_left)
		true_x=delta_left;
	else if(listener_x>delta_right)
		true_x=delta_right;
	if(listener_y<delta_backward)
		true_y=delta_backward;
	else if(listener_y>delta_forward)
		true_y=delta_forward;
	if(listener_z < delta_down)
		true_z = delta_down;
	else if(listener_z > delta_up)
		true_z = delta_up;
	if(listener_x<true_x)
		distance=(true_x-listener_x);
	if(listener_x>true_x)
		distance=(listener_x-true_x);
	if(listener_y<true_y)
		distance+=(true_y-listener_y);
	if(listener_y>true_y)
		distance+=(listener_y-true_y);
	if(listener_z < true_z)
		distance+= true_z-listener_z;
	if(listener_z>true_z)
		distance+= listener_z-true_z;
	return distance;
}
