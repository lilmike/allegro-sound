//sound_pool_example.cpp
//By Michael Taboada
//Provided under the unlicense
//see UNLICENSE for details
/**
*example of sound_pool class usage
*sound_pool allows you to place an arbitrary number of sound sources in virtual space around you, with pitch, volume, and panning adjusted accordingly.
*It fakes 3d with pitch, although that may only be useful to players who are used to that such as those who play audiogames.
*requires allegro core and allegro audio and acodec addons
*assumes you have allegro headers in the standard search path.
*does not do any error checking, see documentation for what each function returns and why.
**/
#include "sound_pool.h"
#include <allegro5/allegro5.h>

int main(int argc, char** argv) {
sound_pool *pool = new sound_pool(4);// we only have 4 sounds, so 2 will save memory.
int sound1 = pool->play_stationary("pool.ogg", true);//the file name, then looping true or false.
al_rest(1);//rest 1 second.
pool->destroy_sound(sound1);//stop the sound and delete it.
int sound2 = pool->play_1d("pool.ogg", 0, 0, true);//file name, the x of the listener, the x of the sound, and looping true or false.
for(int x = -5; x <= 5; x++) {
pool->update_listener_1d(x);// change the listeners x coordinate.
al_rest(0.1);//rest 100 milliseconds.
}
pool->destroy_sound(sound2);
int sound3 = pool->play_2d("pool.ogg", 0, 0, 0, 0, 0, true);//filename, listener x, listener y, listener angle, sound x, sound y, looping
for(int x = -5; x <= 5; x++) {
for(int y = -5; y <= 5; y++) {
pool->update_listener_2d(x, y, 0);//move the listener in x and y, last parameter being angle, not used here.
al_rest(0.1);//rest 100 milliseconds.
}
}
pool->destroy_sound(sound3);
int sound4 = pool->play_3d("pool.ogg", 0, 0, 0, 0, 0, 0, 0, true);//filename, listener x, y, z, angle, sound x, y, z, looping.
for(int x = -5; x <= 5; x++) {
for(int y = -5; y <= 5; y++) {
for(int z = -5; z <= 5; z++) {
pool->update_listener_3d(x, y, z, 0);//listener x, y, z, and angle, angle not used here.
al_rest(0.1);//rest 100 milliseconds.
}
}
}
pool->destroy_sound(sound4);
delete pool;
}
