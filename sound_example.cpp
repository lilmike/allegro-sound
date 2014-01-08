/**
*Example program showing how to load and pan a sound.
*Also shows pitch and gain.
*requires allegro 5 core and audio, and acodec addons.
*assumes that you have the allegro header files in the standard search path.
*does not do any error checking, check the documentation for the classes to see what they return and why.
**/
#include "sound.h"
#include <allegro5/allegro5.h>


int main(int argc, char **argv) {
sound sound1;
sound1.load("test1.ogg"); //loads all formats supported by allegro.
sound1.set_loop(true);//we want this sound to loop, so pass in true.
sound1.play();//plays the sound.
for(int x = -50; x <= 50; x+=5) {
//-50 to 50 is the approximate far left and far right of the speakers.
//technically you can go anywhere from -100 to 100, but most people can't notice past -50 and 50
//uses decibel scale.
sound1.set_pan(x);
//volume goes from -100 to 0, with 0 being full  volume.
//also most people can't tell a difference past -50 to -100.
//also in decibel scale.
sound1.set_gain((x-50)/4);
//pitch goes from 0 to unlimited, only limitation being the machine it's running on.
//probably safe to go from 25 or so to 400 or so.
//50 would be half pitch, 200 would be double pitch, 400 would be quadruple pitch.
//100 is normal pitch.
sound1.set_pitch(x+100);
al_rest(0.1);//rest 100 milliseconds.

}
}
