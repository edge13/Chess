
//===========================================================================
//  File name ......: Sound.cpp
//  Author(s) ......: Justin Hendricks
//  Language .......: C++
//  Started ........: April 11, 2007
//  Last modified ..: May 2, 2007
//  Version ........: 1.0
//  Source Path ....: .
//  Operating System: Linux/Windows
//  Purpose ........: This program creates, plays, stops, and sets the volume
//                    for the background music and sounds in the game.
//  Note ...........: Includes "Sound.h" that stores all the member variables
//                    and method declarations
//===========================================================================

#include "sound.h"

/****************************************************************************
 * Name:        Sounds
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This constructor initializes the channels to 0 and creates
			 and initializes the fmod system object.
 * Invokes:     None
 * Note:        Calls ERRCHECK(FMOD_RESULT result) to ensure all fmod function
			 calls returned a OK status.
 ***************************************************************************/
Sounds::Sounds()
{
	//initialize channels
	channel = 0;
	channel2 = 0;
	
	//Create a system object
	result = FMOD_System_Create(&system2);
	ERRCHECK(result);

	//Initialize the system object
	result = FMOD_System_Init(system2, 32, FMOD_INIT_NORMAL, NULL);
	ERRCHECK(result);
}

/****************************************************************************
 * Name:        ~Sounds
 * Input:       None
 * Output:      None
 * Returns:     None
 * Description: This destructor checks to make sure the FMOD_SOUND pointers,
 *              sound1 and music, are not equal to NULL. If they have been
 *              created then it releases the sound1 and music varibles.
 *              It also closes and releases the system object.
 * Invokes:     None
 * Note:        Calls ERRCHECK(FMOD_RESULT result) to ensure all fmod function
		 	 calls returned a OK status.
 ***************************************************************************/
Sounds::~Sounds()
{
	//Close everything down
	if (sound1)
	{
		result = FMOD_Sound_Release(sound1);
		ERRCHECK(result);
	}

	if (music1)
	{
		result = FMOD_Sound_Release(music1);
		ERRCHECK(result);
	}

	result = FMOD_System_Close(system2);
	ERRCHECK(result);
	result = FMOD_System_Release(system2);
	ERRCHECK(result);
}

/****************************************************************************
 * Name:        ERRCHECK
 * Input:       result -- FMOD_RESULT
 * Output:      Possible error message
 * Returns:     None
 * Description: This method recieves an result variable, which is then checked
 *               to ensure the result was OK. If result contains an error
 *               message, then it is outputted to the console and exits.
 * Invokes:     None
 * Note:        Calls ERRCHECK(FMOD_RESULT result) to ensure all fmod function
		 	 calls returned a OK status
 ***************************************************************************/
void Sounds::ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}

/****************************************************************************
 * Name:        playSound
 * Input:       1. filename -- char*
			 2. loop -- bool
 * Output:      None
 * Returns:     None
 * Description: This function takes in a character string (filename) that
 			 represents the filename of the audio sound to be played and a
			  boolean (loop) determines if the sound should be continously
			  looped or not. It then uses the filename to create a new
			  fmod sound using hardware instead of software and stores the
			  sound object in the sound pointer, sound1. It then checks the
			  boolean loop and if loop is true, sound1 mode is set to loop.
			  Otherwise looping is disabled for sound1. It then plays the
			  sound using channel and updates the system.
 * Invokes:     None
 * Note:        Calls ERRCHECK(FMOD_RESULT result) to ensure all fmod function
		 	 calls returned a OK status
 ***************************************************************************/
void Sounds::playSound(char* filename, bool loop)
{

	//create the sound
    	//result = FMOD_System_CreateSound(system2, filename, FMOD_HARDWARE, 0, &sound1);
	result = FMOD_System_CreateSound(system2, filename, 0, 0, &sound1);
    	ERRCHECK(result);

	//if loop is true, set loop mode, else set loop to off
	if(loop)
		result = FMOD_Sound_SetMode(sound1, FMOD_LOOP_NORMAL);
	else
		result = FMOD_Sound_SetMode(sound1, FMOD_LOOP_OFF);
    	ERRCHECK(result);

	//play the sound using channel
	result = FMOD_System_PlaySound(system2, sound1, 0, 0, &channel);
     ERRCHECK(result);

     FMOD_System_Update(system2);
}

/****************************************************************************
 * Name:        playMusic
 * Input:       1. filename -- char*
			 2. loop -- bool
 * Output:      None
 * Returns:     None
 * Description: This function is essentially the same as playSound, except
			  it uses a different channel to play the sound. (channel2) This
			  function is used to play the background music for the game.
			  Since it is on a different channel, the background music volume
			  can be modified and it can be stopped independently from the
			  sound effects.
 * Invokes:     None
 * Note:        Calls ERRCHECK(FMOD_RESULT result) to ensure all fmod function
		 	 calls returned a OK status
 ***************************************************************************/
void Sounds::playMusic(char* filename, bool loop)
{
     //create the sound
//    	result = FMOD_System_CreateSound(system2, filename, FMOD_HARDWARE, 0, &music1);
	result = FMOD_System_CreateSound(system2, filename, 0, 0, &music1);
    	ERRCHECK(result);

	//if loop is true, set loop mode, else set loop to off
	if(loop)
		result = FMOD_Sound_SetMode(music1, FMOD_LOOP_NORMAL);
	else
		result = FMOD_Sound_SetMode(music1, FMOD_LOOP_OFF);
		
    	ERRCHECK(result);

	//play the sound using channel
	result = FMOD_System_PlaySound(system2, music1, 0, 0, &channel2);
     ERRCHECK(result);

     FMOD_System_Update(system2);
}

/****************************************************************************
 * Name:        setVolume
 * Input:       1. type -- int
			 2. volume -- float
 * Output:      None
 * Returns:     None
 * Description: This function takes in a integer representing either channel (0)
			 or channel2(1). It then sets the volume for that channel using
			 the variable volume, which is a float between 0 and 1.
			 (1 = volume at 100%, 0 = volume at 0%)
 * Invokes:     None
 * Note:        Calls ERRCHECK(FMOD_RESULT result) to ensure all fmod function
		 	 calls returned a OK status
 ***************************************************************************/
void Sounds::setVolume(int type, float volume)
{
	//set sounds volume
	if(type == 0)
		FMOD_Channel_SetVolume(channel, volume);
	//set background music volume
	else
		FMOD_Channel_SetVolume(channel2, volume);
		
    	ERRCHECK(result);
}

/****************************************************************************
 * Name:        stop
 * Input:       1. type -- int
 * Output:      None
 * Returns:     None
 * Description: This function takes in a integer representing either channel (0)
			 or channel2(1). It then stops all sounds/music playing on that
			 channel.
 * Invokes:     None
 * Note:        Calls ERRCHECK(FMOD_RESULT result) to ensure all fmod function
		 	 calls returned a OK status
 ***************************************************************************/
void Sounds::stop(int type)
{
	//stops sound effects
	if(type == 0)
	     FMOD_Channel_Stop(channel);
	//stops background music
	else
	     FMOD_Channel_Stop(channel2);
	     
    	ERRCHECK(result);
}
