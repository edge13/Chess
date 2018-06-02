// Below is a fix for windows
#if defined(WIN32)
#define GLUT_DISABLE_ATEXIT_HACK    //Used to stop a error when compiling on Windows machines
#include <windows.h>
#else
#endif

//===========================================================================
//  File name ......: Sound.h
//  Author(s) ......: Justin Hendricks
//  Language .......: C++
//  Started ........: April 11, 2007
//  Last modified ..: May 2, 2007
//  Version ........: 1.0
//  Source Path ....: .
//  Operating System: Linux/Windows
//  Purpose ........: This file stores all the member variables and method
//				  declarations used to create, plays, stop, and set the
//				  volume for the background music and sounds in the game.
//===========================================================================

#ifndef SOUND_H
#define SOUND_H

#include "fmod/fmod.h"
#include "fmod/fmod_errors.h"
#include <stdio.h>
#include <stdlib.h>

#pragma comment (lib, "fmodex_vc.lib")


/////////////////////////////////////////////////////////////////////////////
// Name:        Sounds
// Description: This class stores all the member variables and method
//			 declarations used to create, plays, stop, and set the
//			 volume for the background music and sounds in the game.
/////////////////////////////////////////////////////////////////////////////
class Sounds
{
	public:
		//Constructor
		Sounds();
		
		//Destructor
		~Sounds();
		
		//checks for any fmod errors
          void ERRCHECK(FMOD_RESULT result);
          
          //plays a sound effect given the filename and a boolean which
		//deterimines if the sound is to be continually looped or not.
          void playSound(char* filename, bool loop);
          
          //plays a sound effect given the filename and a boolean which
		//deterimines if the sound is to be continually looped or not.
          void playMusic(char* filename, bool loop);
          
          //Sets the volume of the sound playing on the channel, type.
          //type 0 = sounds, type 1 = background music
          void setVolume(int type, float volume);
          
          //Stops the sound playing on the channel, type.
          //type 0 = sounds, type 1 = background music
          void stop(int type);
          
	private:
		//Pointer to system variable
		FMOD_SYSTEM *system2;
		
		//Pointer to sound variables
		//sound1 used for sound effects and music used for background music
		FMOD_SOUND *sound1;
		FMOD_SOUND *music1;

		//Pointers to channels used to play audio.
		//channel is used for sounds and channel2 is used for music
		FMOD_CHANNEL *channel, *channel2;

		//error message variable used to store any returned error messages
		FMOD_RESULT result;
};

#endif
