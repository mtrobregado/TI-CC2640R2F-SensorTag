/******************************************************************************

 @file  sensortag_playtune.c

 @brief This file demonstrate how to play tunes using buzzer

 Group: WCS, BTS
 Target Device: CC2650, CC2640, CC1350

 TI CC2640R2F Sensortag using TI CC2640R2F Launchpad + Educational BoosterPack MKII

 Maker/Author - Markel T. Robregado

 Credits to:

 Dung Dang for the Happy Birthday Buzzer Melody
 Dipto Pratyaksa for the Mario Brothers Main Theme and Underworld Melody
 http://www.linuxcircle.com/2013/03/31/playing-mario-bros-tune-with-arduino-and-piezo-buzzer/

 ******************************************************************************/

#include <stdio.h>

#include "gatt.h"
#include "sensortag.h"
#include "sensortag_buzzer.h"
#include "sensortag_playtune.h"
#include "pitches.h"
#include "board.h"
#include "SensorUtil.h"

/*********************************************************************
 * LOCAL VARIABLES
 */
	static uint8_t thisNote;
	static uint8_t noteDuration;
	static uint8_t pauseBetweenNotes;
	static uint8_t arraysize;
 
 /*********************************************************************
 * LOCAL FUNCTIONS
 */
 static void tone(unsigned int frequency, unsigned long duration);
 static void noTone(void);


//Mario main theme melody
int mario_melody[] = {
	NOTE_E7, NOTE_E7, 0, NOTE_E7,
	0, NOTE_C7, NOTE_E7, 0,
	NOTE_G7, 0, 0,  0,
	NOTE_G6, 0, 0, 0,

	NOTE_C7, 0, 0, NOTE_G6,
	0, 0, NOTE_E6, 0,
	0, NOTE_A6, 0, NOTE_B6,
	0, NOTE_AS6, NOTE_A6, 0,

	NOTE_G6, NOTE_E7, NOTE_G7,
	NOTE_A7, 0, NOTE_F7, NOTE_G7,
	0, NOTE_E7, 0, NOTE_C7,
	NOTE_D7, NOTE_B6, 0, 0,

	NOTE_C7, 0, 0, NOTE_G6,
	0, 0, NOTE_E6, 0,
	0, NOTE_A6, 0, NOTE_B6,
	0, NOTE_AS6, NOTE_A6, 0,

	NOTE_G6, NOTE_E7, NOTE_G7,
	NOTE_A7, 0, NOTE_F7, NOTE_G7,
	0, NOTE_E7, 0, NOTE_C7,
	NOTE_D7, NOTE_B6, 0, 0
};

//Mario main them tempo
int mario_tempo[] = {
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,

	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,

	9, 9, 9,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,

	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,

	9, 9, 9,
	12, 12, 12, 12,
	12, 12, 12, 12,
	12, 12, 12, 12,
};

// Mario Underworld melody
int mario_underworld_melody[] = {
	NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
	NOTE_AS3, NOTE_AS4, 0,
	0,
	NOTE_C4, NOTE_C5, NOTE_A3, NOTE_A4,
	NOTE_AS3, NOTE_AS4, 0,
	0,
	NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
	NOTE_DS3, NOTE_DS4, 0,
	0,
	NOTE_F3, NOTE_F4, NOTE_D3, NOTE_D4,
	NOTE_DS3, NOTE_DS4, 0,
	0, NOTE_DS4, NOTE_CS4, NOTE_D4,
	NOTE_CS4, NOTE_DS4,
	NOTE_DS4, NOTE_GS3,
	NOTE_G3, NOTE_CS4,
	NOTE_C4, NOTE_FS4, NOTE_F4, NOTE_E3, NOTE_AS4, NOTE_A4,
	NOTE_GS4, NOTE_DS4, NOTE_B3,
	NOTE_AS3, NOTE_A3, NOTE_GS3,
	0, 0, 0
};

//Mario Underworld tempo
int mario_underworld_tempo[] = {
	12, 12, 12, 12,
	12, 12, 6,
	3,
	12, 12, 12, 12,
	12, 12, 6,
	3,
	12, 12, 12, 12,
	12, 12, 6,
	3,
	12, 12, 12, 12,
	12, 12, 6,
	6, 18, 18, 18,
	6, 6,
	6, 6,
	6, 6,
	18, 18, 18, 18, 18, 18,
	10, 10, 10,
	10, 10, 10,
	3, 3, 3
}; 
 
// notes in the Happy Birthday melody:
int birthdaymelody[] = {
	NOTE_C4_1, NOTE_C4, NOTE_D4, NOTE_C4,NOTE_F4, NOTE_E4,
	NOTE_C4_1, NOTE_C4, NOTE_D4, NOTE_C4, NOTE_G4, NOTE_F4,
	NOTE_C4_1, NOTE_C4, NOTE_C5, NOTE_A4, NOTE_F4, NOTE_F4,
	NOTE_E4, NOTE_D4, NOTE_AS4, NOTE_AS4, NOTE_A4, NOTE_F4,
	NOTE_G4,NOTE_F4
};
   
   
// Happy Birthday note durations: 4 = quarter note, 
// 8 = eighth note, etc.:
int birthdaynoteDurations[] = {
    4, 4, 2, 2, 2, 1,
    4, 4, 2, 2, 2, 1,
    4, 4, 2, 2, 4, 4,
    2, 1, 4, 4, 2, 2,
    2, 1
};

void playbirthdaytune(void)
{	
	for(thisNote = 0; thisNote < 26; thisNote++)
	{
		// to calculate the note duration, take one second 
		// divided by the note type.
		//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
		noteDuration = 1000 / birthdaynoteDurations[thisNote];
		tone(birthdaymelody[thisNote], noteDuration);

		pauseBetweenNotes = noteDuration + 70;      //delay between pulse
		DELAY_MS(pauseBetweenNotes);
    
		// stop the tone playing
		noTone();                
	}
}

void playmariotune(void)
{	
	arraysize = sizeof(mario_melody) / sizeof(int);
	
	for (thisNote = 0; thisNote < arraysize; thisNote++)
	{

		// to calculate the note duration, take one second
		// divided by the note type.
		//e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
		noteDuration = 1000 / mario_tempo[thisNote];
		tone(mario_melody[thisNote], noteDuration);

		// to distinguish the notes, set a minimum time between them.
		// the note's duration + 30% seems to work well:
		pauseBetweenNotes = noteDuration * 1.30;
		DELAY_MS(pauseBetweenNotes);

		// stop the tone playing:
		noTone();                
	}
}

void playmariounderworldtune(void)
{	
    arraysize = sizeof(mario_underworld_melody) / sizeof(int);
	
	for (thisNote = 0; thisNote < arraysize; thisNote++)
	{

       // to calculate the note duration, take one second
       // divided by the note type.
       //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
       int noteDuration = 1000 / mario_underworld_tempo[thisNote];
       tone(mario_underworld_melody[thisNote], noteDuration);

       // to distinguish the notes, set a minimum time between them.
       // the note's duration + 30% seems to work well:
       pauseBetweenNotes = noteDuration * 1.30;
       DELAY_MS(pauseBetweenNotes);

       // stop the tone playing:
       noTone();                
    }
}

void tone(unsigned int frequency, unsigned long duration)
{
	SensorTagBuzzer_open(hGpioPin);
    SensorTagBuzzer_setFrequency(frequency);
	DELAY_MS(duration);
}

void noTone(void)
{
	SensorTagBuzzer_setFrequency(0);
}	
	
