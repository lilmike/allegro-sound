#include "sound_pool.h"

sound_pool::sound_pool(void)
{
	items = new sound_pool_item[100];
	numberofitems = 100;
	max_distance=0;
	pan_step=1.0;
	volume_step=1.0;
	behind_pitch_decrease=0.5;
	behind_volume_decrease = 5.0;
	above_pitch = 5.0;
	last_listener_x=0;
	last_listener_y=0;
	last_listener_z = 0;
	last_listener_angle=0;
}

sound_pool::sound_pool(int numberofitems) {
	if(numberofitems > 0) {
		items = new sound_pool_item[numberofitems];
		numberofitems = numberofitems;
	}
	else {
		items = new sound_pool_item[100];
		numberofitems=100;
	}
	max_distance=0;
	pan_step=1.0;
	volume_step=1.0;
	behind_pitch_decrease=0.5;
	behind_volume_decrease = 5.0;
	above_pitch = 5.0;
	last_listener_x=0;
	last_listener_y=0;
	last_listener_z = 0;
	last_listener_angle=0;
}

sound_pool::~sound_pool(void)
{
delete[] items;
}

int sound_pool::play_stationary(string filename, bool looping) {
	return play_stationary_extended(filename, looping, 0, 0, 100, false);
}

int sound_pool::play_stationary_extended(string filename, bool looping, double start_pan, double start_volume, double start_pitch, bool persistent) {
	clean_unused();
	int slot=reserve_slot(filename);
	if(slot==-1) {
		return -1;
	}
	items[slot].filename=filename;
	items[slot].looping=looping;
	items[slot].stationary=true;
	items[slot].start_pan=start_pan;
	items[slot].start_volume=start_volume;
	items[slot].start_pitch=start_pitch;
	items[slot].persistent=persistent;
	items[slot].s->load(items[slot].filename);
	if(!items[slot].s->get_active()) {
		items[slot].reset();
		return -1;
	}
	if(start_pan!=0.0) {
		items[slot].s->set_pan(start_pan);
	}
	if(start_volume<0) {
		items[slot].s->set_gain(start_volume);
	}
	items[slot].s->set_pitch(start_pitch);
	if(looping==true) {
		items[slot].s->set_loop(true);
		items[slot].s->play();
	}
	else{
		items[slot].s->play();
	}
	return slot;
}

int sound_pool::play_1d(string filename, int listener_x, int sound_x, bool looping) {
	return play_extended_1d(filename, listener_x, sound_x, 0, 0, looping, 0, 0, 100, false);
}

int sound_pool::play_extended_1d(string filename, int listener_x, int sound_x, int left_range, int right_range, bool looping, double start_pan, double start_volume, double start_pitch, bool persistent) {
	clean_unused();
	int slot=reserve_slot(filename);
	if(slot==-1)
	{
		return -1;
	}
	items[slot].filename=filename;
	items[slot].x=sound_x;
	items[slot].y=0;
	items[slot].z = 0;
	items[slot].looping=looping;
	items[slot].pan_step=pan_step;
	items[slot].volume_step=volume_step;
	items[slot].behind_pitch_decrease=0.0;
	items[slot].behind_volume_decrease = behind_volume_decrease;
	items[slot].above_pitch=above_pitch;
	items[slot].start_pan=start_pan;
	items[slot].start_volume=start_volume;
	items[slot].start_pitch=start_pitch;
	items[slot].left_range=left_range;
	items[slot].right_range=right_range;
	items[slot].backward_range=0;
	items[slot].forward_range=0;
	items[slot].up_range=0;
	items[slot].down_range=0;
	items[slot].is_2d = true;
	items[slot].is_3d=false;
	items[slot].persistent=persistent;
	if(max_distance>0 && items[slot].get_total_distance(listener_x, 0, 0)>max_distance) {
		if(looping==false) {
			items[slot].reset();
			return -2;
		}
		else {
			last_listener_x=listener_x;
			items[slot].s->set_pitch(start_pitch);
			items[slot].update(listener_x, 0, 0, 0, max_distance);
			return slot;
		}
	}
	items[slot].s->load(items[slot].filename);
	if(!items[slot].s->get_active()) {
		items[slot].reset();
		return -1;
	}
	items[slot].s->set_pitch(start_pitch);
	last_listener_x=listener_x;
	items[slot].update(listener_x, 0, 0, 0, max_distance);
	if(looping==true) {
		items[slot].s->set_loop(true);
		items[slot].s->play();
	}
	else {
		items[slot].s->play();
	}
	return slot;
}

int sound_pool::play_extended_2d(string filename, int listener_x, int listener_y, double listener_angle, int sound_x, int sound_y, int left_range, int right_range, int backward_range, int forward_range, bool looping, float start_pan, float start_volume, float start_pitch, bool persistent) {
	clean_unused();
	int slot=reserve_slot(filename);
	if(slot==-1) {
		return -1;
	}
	items[slot].filename=filename;
	items[slot].x=sound_x;
	items[slot].y=sound_y;
	items[slot].z = 0;
	items[slot].looping=looping;
	items[slot].pan_step=pan_step;
	items[slot].volume_step=volume_step;
	items[slot].behind_pitch_decrease=behind_pitch_decrease;
	items[slot].behind_volume_decrease = behind_volume_decrease;
	items[slot].above_pitch = above_pitch;
	items[slot].start_pan=start_pan;
	items[slot].start_volume=start_volume;
	items[slot].start_pitch=start_pitch;
	items[slot].left_range=left_range;
	items[slot].right_range=right_range;
	items[slot].backward_range=backward_range;
	items[slot].forward_range=forward_range;
	items[slot].up_range = 0;
	items[slot].down_range = 0;
	items[slot].is_2d=true;
	items[slot].is_3d=false;
	items[slot].persistent=persistent;
	if(max_distance>0 && items[slot].get_total_distance(listener_x, listener_y, 0)>max_distance) {
		if(looping==false) {
			items[slot].reset();
			return -2;
		}
		else {
			last_listener_x=listener_x;
			last_listener_y=listener_y;
			last_listener_angle = listener_angle;
			items[slot].update(listener_x, listener_y, 0, listener_angle, max_distance);
			return slot;
		}
	}
	items[slot].s->load(items[slot].filename);
	if(!items[slot].s->get_active()) {
		items[slot].reset();
		return -1;
	}
	last_listener_x=listener_x;
	last_listener_y=listener_y;
	last_listener_angle = listener_angle;
	items[slot].update(listener_x, listener_y, 0, listener_angle, max_distance);
	if(looping==true) {
		items[slot].s->set_loop(true);
		items[slot].s->play();
	}
	else {
		items[slot].s->play();
	}
	return slot;
}

int sound_pool::play_sound_2d(string filename, int listener_x, int listener_y, double listener_angle, int sound_x, int sound_y, bool looping) {
	return play_extended_2d(filename, listener_x, listener_y, listener_angle, sound_x, sound_y, 0, 0, 0, 0, looping, 0, 0, 100, false);
}

int sound_pool::play_sound_3d(string filename, int listener_x, int listener_y ,int listener_z, double listener_angle, int sound_x, int sound_y, int sound_z, bool looping) {
	return play_extended_3d(filename, listener_x, listener_y, listener_z, listener_angle, sound_x, sound_y, sound_z, 0, 0, 0, 0, 0, 0, looping, 0, 0, 100, false);
}

int sound_pool::play_extended_3d(string filename, int listener_x, int listener_y, int listener_z, double listener_angle, int sound_x, int sound_y, int sound_z, int left_range, int right_range, int backward_range, int forward_range, int up_range, int down_range, bool looping, float start_pan, float start_volume, float start_pitch, bool persistent) {
	clean_unused();
	int slot=reserve_slot(filename);
	if(slot==-1) {
		return -1;
	}
	items[slot].filename=filename;
	items[slot].x=sound_x;
	items[slot].y=sound_y;
	items[slot].z = sound_z;
	items[slot].looping=looping;
	items[slot].pan_step=pan_step;
	items[slot].volume_step=volume_step;
	items[slot].behind_pitch_decrease=behind_pitch_decrease;
	items[slot].behind_volume_decrease = behind_volume_decrease;
	items[slot].above_pitch = above_pitch;
	items[slot].start_pan=start_pan;
	items[slot].start_volume=start_volume;
	items[slot].start_pitch=start_pitch;
	items[slot].left_range=left_range;
	items[slot].right_range=right_range;
	items[slot].backward_range=backward_range;
	items[slot].forward_range=forward_range;
	items[slot].up_range = up_range;
	items[slot].down_range = down_range;
	items[slot].is_2d=true;
	items[slot].is_3d=true;
	items[slot].persistent=persistent;
	if(max_distance>0 && items[slot].get_total_distance(listener_x, listener_y, listener_z)>max_distance) {
		if(looping==false) {
			items[slot].reset();
			return -2;
		}
		else {
			last_listener_x=listener_x;
			last_listener_y=listener_y;
			last_listener_z = listener_z;
			last_listener_angle = listener_angle;
			items[slot].update(listener_x, listener_y, listener_z, listener_angle, max_distance);
			return slot;
		}
	}
	items[slot].s->load(items[slot].filename);
	if(!items[slot].s->get_active()) {
		items[slot].reset();
		return -1;
	}
	last_listener_x=listener_x;
	last_listener_y=listener_y;
	last_listener_z = listener_z;
	last_listener_angle = listener_angle;
	items[slot].update(listener_x, listener_y, listener_z, listener_angle, max_distance);
	if(looping==true) {
		items[slot].s->set_loop(true);
		items[slot].s->play();
	}
	else {
	items[slot].s->play();
	}
	return slot;
}

bool sound_pool::sound_is_active(int slot) {
	if(verify_slot(slot)==false) {
		return false;
	}
	if(items[slot].looping==false && items[slot].s==NULL) {
		return false;
	}
	if(items[slot].looping==false && items[slot].s->is_playing()==false) {
		return false;
	}
	return true;
}

bool sound_pool::sound_is_playing(int slot) {
	if(sound_is_active(slot)==false) {
		return false;
	}
	return items[slot].s->is_playing();
}

bool sound_pool::pause_sound(int slot) {
	if(sound_is_active(slot)==false) {
		return false;
	}
	if(items[slot].paused==true) {
		return false;
	}
	items[slot].paused=true;
	items[slot].s->pause();
	return true;
}

bool sound_pool::resume_sound(int slot) {
	if(verify_slot(slot)==false) {
		return false;
	}
	if(items[slot].paused==false) {
		return false;
	}
	items[slot].paused=false;
	if(max_distance>0 && items[slot].get_total_distance(last_listener_x, last_listener_y, last_listener_z)>max_distance) {
		if(items[slot].s->get_active()) {
			delete items[slot].s;
			items[slot].s = NULL;
		}
		return true;
	}
	items[slot].update(last_listener_x, last_listener_y, last_listener_z, last_listener_angle, max_distance);
	if(items[slot].s!=NULL && items[slot].s->is_playing() == false) {
		if(items[slot].looping==true) {
			items[slot].s->set_loop(true);
			items[slot].s->play();
		}
		else {
			items[slot].s->play();
		}
	}
	return true;
}

template<typename T, int size>
int getArrLength(T(&)[size]){return size;}


void sound_pool::pause_all() {
	int currently_playing=0;
	for(int i=0; i<numberofitems; i++) {
		if(sound_is_playing(i))
			currently_playing+=1;
		pause_sound(i);
	}
}

void sound_pool::resume_all() {
	int currently_playing=0;
	for(int i=0; i<numberofitems; i++) {
		resume_sound(i);
		if(sound_is_playing(i))
			currently_playing+=1;
	}
}

void sound_pool::destroy_all() {
	for(int i=0; i<numberofitems; i++) {
		items[i].reset();
	}
}

void sound_pool::update_listener_1d(int listener_x) {
	update_listener_2d(listener_x, 0, 0);
}

void sound_pool::update_listener_2d(int listener_x, int listener_y, double listener_angle) {
	update_listener_3d(listener_x, listener_y, 0, listener_angle);
}

void sound_pool::update_listener_3d(int listener_x, int listener_y, int listener_z, double listener_angle) {
	last_listener_x=listener_x;
	last_listener_y=listener_y;
	last_listener_z = listener_z;
	last_listener_angle= listener_angle;
	clean_unused();
	for(int i=0; i<numberofitems; i++) {
		items[i].update(listener_x, listener_y, listener_z, listener_angle, max_distance);
	}
}

bool sound_pool::update_sound_1d(int slot, int x) {
	return update_sound_2d(slot, x, 0);
}

bool sound_pool::update_sound_2d(int slot, int x, int y) {
	return update_sound_3d(slot, x, y, 0);
}

bool sound_pool::update_sound_3d(int slot, int x, int y, int z) {
	if(verify_slot(slot)==false) {
		return false;
	}
	items[slot].x=x;
	items[slot].y=y;
	items[slot].z=z;
	items[slot].update(last_listener_x, last_listener_y, last_listener_z, last_listener_angle, max_distance);
	return true;
}

bool sound_pool::update_sound_start_values(int slot, float start_pan, float start_volume, float start_pitch) {
	if(verify_slot(slot)==false) {
		return false;
	}
	items[slot].start_pan=start_pan;
	items[slot].start_volume=start_volume;
	items[slot].start_pitch=start_pitch;
	items[slot].update(last_listener_x, last_listener_y, last_listener_z, last_listener_angle, max_distance);
	if(items[slot].stationary==true && items[slot].s!=NULL) {
		items[slot].s->set_pan(start_pan);
		items[slot].s->set_gain(start_volume);
		items[slot].s->set_pitch(start_pitch);
		return true;
	}
	if(items[slot].is_2d==false && items[slot].s->get_pitch()!=start_pitch) {
		items[slot].s->set_pitch(start_pitch);
	}
	return true;
}

bool sound_pool::update_sound_range_1d(int slot, int left_range, int right_range) {
	return update_sound_range_2d(slot ,left_range, right_range, 0, 0);
}

bool sound_pool::update_sound_range_2d(int slot, int left_range, int right_range, int backward_range, int forward_range) {
	return update_sound_range_3d(slot, left_range, right_range, backward_range, forward_range, 0, 0);
}

bool sound_pool::update_sound_range_3d(int slot, int left_range, int right_range, int backward_range, int forward_range, int up_range, int down_range) {
	if(verify_slot(slot)==false) {
		return false;
	}
	items[slot].left_range=left_range;
	items[slot].right_range=right_range;
	items[slot].backward_range=backward_range;
	items[slot].forward_range=forward_range;
	items[slot].up_range=up_range;
	items[slot].down_range=down_range;
	items[slot].update(last_listener_x, last_listener_y, last_listener_z, last_listener_angle, max_distance);
	return true;
}

bool sound_pool::destroy_sound(int slot) {
	if(verify_slot(slot) == true) {
		items[slot].reset();
		return true;
	}
	return false;
}

void sound_pool::clean_unused() {
	for(int i=0; i<numberofitems; i++) {
		if(items[i].persistent==true) {
			continue;
		}
		if(items[i].looping==true) {
			continue;
		}
		if(items[i].s==NULL) {
			continue;
		}
		if(items[i].s->get_active()==false) {
			continue;
		}
		if(items[i].s->is_playing()==false && items[i].paused==false) {
			items[i].reset();
		}
	}
}

bool sound_pool::verify_slot(int slot) {
	if(slot<0) {
		return false;
	}
	if(slot>=numberofitems) {
		return false;
	}
	if(items[slot].persistent==true) {
		return true;
	}
if(items[slot].looping==true) {
		return true;
	}
		if(items[slot].s!=NULL) {
	return true;
	}
	return false;
}

int sound_pool::reserve_slot(string filename) {
	int slot=-1;
	for(int i=0; i<numberofitems; i++) {
		if(items[i].persistent==true) {
			continue;
		}
		if(items[i].looping==true) {
			continue;
		}
		if(items[i].s==NULL) {
			slot=i;
			break;
		}
		if(items[i].s->get_active()==false) {
			slot=i;
			break;
		}
		if(items[i].s->is_playing()==false) {
			slot=i;
			break;
		}
	}
	if(slot==-1) {
		return -1;
	}
	items[slot].reset();
	items[slot].s = new sound();
	return slot;
}
