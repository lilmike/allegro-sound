//sound_pool.cpp
//By Michael Taboada
//Provided under the unlicense
//see UNLICENSE for details
#include "sound_pool.h"

/**
*Sets up a sound pool with 100 slots.
**/
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

/**
*Sets up a sound pool with numberofitems slots.
**/
sound_pool::sound_pool(int numberofitems/**< [in] the number of slots the sound pool can hold. **/) {
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

/**
*Plays a sound that doesn't move.
*@return The slot that was created for the sound.
**/
int sound_pool::play_stationary(string filename/**< [in] The file to load.**/, bool looping/**< [in] Whether the sound loops.**/) {
	return play_stationary_extended(filename, looping, 0, 0, 100, false);
}

/**
*Plays a sound that doesn't move, but has starting values for pan, volume, and pitch.
*@return The slot that was created for the sound.
**/
int sound_pool::play_stationary_extended(string filename/**< [in] The file to play.**/, bool looping/**< [in] Whether the sound loops.**/, double start_pan/**< [in] What to modify the pan by.**/, double start_volume/**< [in] What to modify the volume by.**/, double start_pitch/**< [in] What to modify the pitch by.**/, bool persistent/**< [in] Should the slot be cleaned up when the file ends if not looping?**/) {
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

/**
*plays a sound that moves in the x axis.
*@return The slot that was created for the sound.
**/
int sound_pool::play_1d(string filename/**< [in] The file to play.**?, int listener_x/**< [in] The x coordinate of the listener.**/, int sound_x/**< [in] The x coordinate of the sound.**/, bool looping/**< Will the file loop?**/) {
	return play_extended_1d(filename, listener_x, sound_x, 0, 0, looping, 0, 0, 100, false);
}

/**
*Allows you to play a sound with beginning pitch pan and volume adjustments.
*@return The slot that was created for the sound.
**/
int sound_pool::play_extended_1d(string filename/**< [in] The file to play.**/, int listener_x/**< [in] The listener's x coordinate.**/, int sound_x/** [in] The sound's x coordinate.**/, int left_range/**< [in] The x coordinate on the left from which the sound should start moving, for example when crossing a river it will stay in the center of your speaker.**/, int right_range/**< [in] The right coordinate from which the  sound should start moving.**/, bool looping/**< [in] Should the file loop?**/, double start_pan/**< [in] The pan value to modify the sound by.**/, double start_volume/**< [in] The volume value to modify the sound by.**/, double start_pitch/**< [in] The pitch value to modify the sound by.**/, bool persistent/**< [in] Should the flie be cleaned up when it it done playing if no looping.**/) {
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

/**
*Play a sound in the 2d x-y plane, with starting values and ranges defined.
*@return The slot that was created for the sound.
**/
int sound_pool::play_extended_2d(string filename/**< [in] The file to play.**/, int listener_x/**< [in] The x coordinate of the listener.**/, int listener_y/**< [in] The y coordintae of the listener.**/, double listener_angle/**< [in] The angle the listener is facing.**/, int sound_x/**< [in] The x coordinate of the sound.**/, int sound_y/**< [in] The y coordinate of the sound.**/, int left_range/**< [in] The left x coordinate at which the sound starts moving.**/, int right_range/**< [in] The right x coordinate at which the sound starts moving.**/, int backward_range/**< [in] The backward y coordinate at which the sound starts moving.**/, int forward_range/**< [in] The forward y coordinate at which the sound starts moving.**/, bool looping/**< [in] Does the file loop?**/, float start_pan/**< [in] The pan value to modify the sound by.**/, float start_volume/**< [in] The volume value to modify the sound by.**/, float start_pitch/**< [in] The pitch value to modify the sound by.**/, bool persistent/**< [in] Should the file be cleaned up when it is done playing if not looping?**/) {
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

/**
*Plays a sound in the 2d x-y plane.
*@return The slot that was created for the sound.
**/
int sound_pool::play_2d(string filename/**< [in] The file to play.**/, int listener_x/**< [in] The x coordintae of the listener.**/, int listener_y/**< [in] The y coordinate of the listener.**/, double listener_angle/**< [in] The angle the listener is facing.**/, int sound_x/** [in] The x coordinate of the sound.**/, int sound_y/**< [in] The y coordinate of the sound.**/, bool looping/**< [in] Should the file loop?**/) {
	return play_extended_2d(filename, listener_x, listener_y, listener_angle, sound_x, sound_y, 0, 0, 0, 0, looping, 0, 0, 100, false);
}

/**
*Plays a sound in 3d.
*@return The slot that was created for the sound.
**/
int sound_pool::play_3d(string filename/**< [in] The file to play.**/, int listener_x/**< [in] The listener's x coordinate.**/, int listener_y/**< [in] The listener's y coordinate.**/,int listener_z/**< [in] The listener's z coordinate.**/, double listener_angle/**< [in] The angle the listener is facing.**/, int sound_x/**< [in] The sound's x coordinate.**/, int sound_y/**< [in] The sounds y coordinate.**/, int sound_z/**< [in] The sounds z coordinate.**/, bool looping/**< [in] Does the sound loop?**/) {
	return play_extended_3d(filename, listener_x, listener_y, listener_z, listener_angle, sound_x, sound_y, sound_z, 0, 0, 0, 0, 0, 0, looping, 0, 0, 100, false);
}

/**
*Plays a sound in 3d with starting values for pitch, volume, pan, and ranges.
*@return The slot that was created for the sound.
**/
int sound_pool::play_extended_3d(string filename/**< [in] The file to play.**/, int listener_x/**< [in] The listener's x coordinate.**/, int listener_y/**< [in] The listener's y coordinate.**/, int listener_z/**< [in] The listener's z coordinate.**/, double listener_angle/**< [in] The listener's facing angle.**/, int sound_x/**< [in] The x coordinate of the sound.**/, int sound_y/**< [in] The y coordinate of the sound.**/, int sound_z/**< [in] The z coordinate of the sound.**/, int left_range/**< [in] The left x at which the sound starts moving.**/, int right_range/**< [in] The right x at which the sound starts moving.**/, int backward_range/**< [in] The backward y coordinate at which the sound starts moving.**/, int forward_range/**< [in] The forward y at which the sound starts moving.**/, int up_range/**< [in] The upward z at which the sound starts moving.**/, int down_range/**< [in] The downward z at which the sound starts moving.**/, bool looping/**< [in] Does the file loop?**/, float start_pan/**< [in] The starting pan value.**/, float start_volume/**< [in] The starting volume value.**/, float start_pitch/**< [in] The starting pitch value.**/, bool persistent/**< [in] Does the slot get cleaned up after the file stops playing if not looping?**/) {
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

/**
*Check if a slot is active.
*@return a boolean indicating whether the slot is active.
**/
bool sound_pool::sound_is_active(int slot/**< [in] The slot to check.**/) {
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

/**
*Is the sound at this slot playing?
*@return Whether the sound is playing.
**/
bool sound_pool::sound_is_playing(int slot/**< [in] The slot to check.**/) {
	if(sound_is_active(slot)==false) {
		return false;
	}
	return items[slot].s->is_playing();
}

/**
*Pauses the sound.
*@return Whether the sound was paused.
**/
bool sound_pool::pause_sound(int slot/**< [in] The slot of the sound to pause.**/) {
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

/**
*Unpause a previously paused sound.
*@return Whether the sound was unpaused.
**/
bool sound_pool::resume_sound(int slot/**< [in] The slot to resume.**/) {
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

/**
*Pauses all playing sounds.
**/
void sound_pool::pause_all() {
	int currently_playing=0;
	for(int i=0; i<numberofitems; i++) {
		if(sound_is_playing(i))
			currently_playing+=1;
		pause_sound(i);
	}
}

/**
*Resumes all paused sounds.
**/
void sound_pool::resume_all() {
	int currently_playing=0;
	for(int i=0; i<numberofitems; i++) {
		resume_sound(i);
		if(sound_is_playing(i))
			currently_playing+=1;
	}
}

/**
*Stops all sounds from playing.
**/
void sound_pool::destroy_all() {
	for(int i=0; i<numberofitems; i++) {
		items[i].reset();
	}
}

/**
*Updates the x position of the listener.
**/
void sound_pool::update_listener_1d(int listener_x/**< [in] The new x position of the listener.**/) {
	update_listener_2d(listener_x, 0, 0);
}

/**
*Updates the listener's x, y, and facing angles.
**/
void sound_pool::update_listener_2d(int listener_x/**< [in] The listener's x coordinate.**/, int listener_y/**< [in] The listener's y coordinate.**/, double listener_angle/**< [in] The listener's facing angle.**/) {
	update_listener_3d(listener_x, listener_y, 0, listener_angle);
}

/**
*Updates the listener's x, y,z, and facing angle.
**/
?void sound_pool::update_listener_3d(int listener_x/**< [in] The listener's x coordinate.**/, int listener_y/**< [in] The listener's y coordinate.**/, int listener_z/**< [in] The listener's z coordinate.**/, double listener_angle/**< [in] The listener's facing angle.**/) {
	last_listener_x=listener_x;
	last_listener_y=listener_y;
	last_listener_z = listener_z;
	last_listener_angle= listener_angle;
	clean_unused();
	for(int i=0; i<numberofitems; i++) {
		items[i].update(listener_x, listener_y, listener_z, listener_angle, max_distance);
	}
}

/**
*Updates a playing sound's x coordintae.
*@return Whether the sound was successfully updated.
**/
bool sound_pool::update_sound_1d(int slot/**< [in] The slot to update.**/, int x/**< [in] The x coordinate.**/) {
	return update_sound_2d(slot, x, 0);
}

/**
*updates the sound's x and y coordinates.
*@return Whether the sound was successfully updated.
**/
bool sound_pool::update_sound_2d(int slot/**< [in] The slot to update.**/, int x/**< [in] The x coordinate.**/, int y/**< [in] The y coordinate.**/) {
	return update_sound_3d(slot, x, y, 0);
}

/**
*Updates the sound's x, y, and z coordinates.
*@return Whether the sound was successfully updated.
**/
bool sound_pool::update_sound_3d(int slot/**< [in] The slot to update.**/, int x/**< [in] The x coordinate.**/, int y/*< [in The y coordinate.**/, int z/**< [in] The z coordinate.**/) {
	if(verify_slot(slot)==false) {
		return false;
	}
	items[slot].x=x;
	items[slot].y=y;
	items[slot].z=z;
	items[slot].update(last_listener_x, last_listener_y, last_listener_z, last_listener_angle, max_distance);
	return true;
}

/**
*Updates the starting values to modify the sound by.
*@return Whether the sound was successfully updated.
**/
bool sound_pool::update_sound_start_values(int slot/**< [in] The slot to update.**/, float start_pan/**< [in] The starting pan.**/, float start_volume/**< [in] The starting volume.**/, float start_pitch/**< [in] The starting pitch.**/) {
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

/**
*Updates the x ranges for the sound.
*@return Whether the sound was successfully updated.
**/
bool sound_pool::update_sound_range_1d(int slot/**< [in] The slot to update.**/, int left_range/**< [in] The left x coordinate to start moving the sound at.**/, int right_range/**< [in] The right x cordinate to start moving the sound at.**/) {
	return update_sound_range_2d(slot ,left_range, right_range, 0, 0);
}

/**
*Updates the x and y ranges for the sound.
*@return Whether the sound was successfully updated.
**/
bool sound_pool::update_sound_range_2d(int slot/**< [in] The slot to update.**/, int left_range/**< [in] The left x coordinate to start moving the sound at.**/, int right_range/**< [in] The right x coordinate to start moving the sound at.**/, int backward_range/**< [in] The back y coordinate to start moving the sound at.**/, int forward_range/**< [in] The front y coordinate to start moving the sound at.**/) {
	return update_sound_range_3d(slot, left_range, right_range, backward_range, forward_range, 0, 0);
}

/**
*Updates the x, y, and z ranges for the sound.
*@return Whether the sound was successfully updated.
**/
bool sound_pool::update_sound_range_3d(int slot/**< [in] The slot to update.**/, int left_range/**< [in the left x coordinate to start moving the sound at.**/, int right_range/**< [in] The right x coordinate to start moving the sound at.**/, int backward_range/**< [in] The back y coordinate to start moving the sound at.**/, int forward_range/**< [in] The front y coordinate to start moving the sound at.**/, int up_range/**< [in] The upward range to start moving the sound at.**/, int down_range/**< [in] The downward range to start moving the sound at.**/) {
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

/**
*Stop playing a sound and destroy the slot.
*@return Whether the sound was successfully stoped.
**/
bool sound_pool::destroy_sound(int slot/**< [in] The slot to stop.**/) {
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
