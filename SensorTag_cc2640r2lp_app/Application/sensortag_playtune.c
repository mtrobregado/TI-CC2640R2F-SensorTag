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

int konami_melody[] = {
	NOTE_F2, NOTE_C6, NOTE_B5, NOTE_G5, NOTE_A5, NOTE_E1, NOTE_B1, NOTE_E1, NOTE_B1, NOTE_E1, NOTE_B1,    

	NOTE_G6, NOTE_F6, NOTE_DS6, NOTE_C6, NOTE_AS5, NOTE_C6, NOTE_AS5, NOTE_GS5, NOTE_G5, NOTE_GS5, 
	NOTE_G5, NOTE_F5, NOTE_DS5, NOTE_F5, NOTE_AS4, NOTE_C5, NOTE_DS5, NOTE_F5,

	NOTE_C6, NOTE_NOTONE, NOTE_AS5, NOTE_C6, NOTE_D6, NOTE_DS6, NOTE_F5,
	NOTE_C6, NOTE_NOTONE, NOTE_AS5, NOTE_C6, NOTE_D6, NOTE_GS5, NOTE_F5,
	// Repeat once   
	NOTE_C6, NOTE_NOTONE, NOTE_AS5, NOTE_C6, NOTE_D6, NOTE_DS6, NOTE_F5,
	NOTE_C6, NOTE_NOTONE, NOTE_AS5, NOTE_C6, NOTE_D6, NOTE_GS5, NOTE_F5,

	NOTE_C6, NOTE_NOTONE, NOTE_C6, NOTE_D6, NOTE_DS6, NOTE_NOTONE, NOTE_DS6, NOTE_NOTONE,
	NOTE_G5, NOTE_AS5, NOTE_C6, NOTE_D6, NOTE_NOTONE, NOTE_D6, NOTE_DS6,

	NOTE_C6, NOTE_NOTONE, NOTE_C6, NOTE_NOTONE, NOTE_DS6, NOTE_F6  
};


int konaminoteDurations[] = {
	2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 1,         // 11 start screen  notes

	16, 16, 16, 16, 16, 16, 16, 16, 16, 16,  // 20  notes
	16, 16, 16, 16, 16, 16, 16, 2,           // for both lines   

	16, 16, 16, 8, 16, 2, 2,                 // 7
	16, 16, 16, 8, 16, 2, 2,                 // 7

	// Repeat once
	16, 16, 16, 8, 16, 2, 2,                 // 7
	16, 16, 16, 8, 16, 2, 2,                 // 7

	16, 16, 4, 4, 16, 16, 16, 8,             // 8
	16, 16, 16, 16, 16, 4, 4,                // 7

	16, 16, 16, 16, 16, 4                    // 6 
};

int got_melody[] = {
    NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,
    NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,

    NOTE_G4, NOTE_C4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_E4, NOTE_F4,
    NOTE_G4, NOTE_C4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_E4, NOTE_F4,

    NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_D4,

    NOTE_G3, NOTE_AS3, NOTE_C4, NOTE_D4, NOTE_G3, NOTE_AS3, NOTE_C4, NOTE_D4,
    NOTE_G3, NOTE_AS3, NOTE_C4, NOTE_D4,

    NOTE_G3, NOTE_AS3, NOTE_C4, NOTE_D4,

    NOTE_F4, NOTE_AS3, NOTE_DS4, NOTE_D4,

    NOTE_F4, NOTE_AS3, NOTE_DS4, NOTE_D4, NOTE_C4,

    NOTE_GS3, NOTE_AS3, NOTE_C4, NOTE_F3, NOTE_GS3, NOTE_AS3, NOTE_C4, NOTE_F3,
    NOTE_GS3, NOTE_AS3, NOTE_C4, NOTE_F3,

    NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,
    NOTE_D4,

    NOTE_G3, NOTE_AS3, NOTE_C4, NOTE_D4, NOTE_G3, NOTE_AS3, NOTE_C4, NOTE_D4,
    NOTE_G3, NOTE_AS3, NOTE_C4, NOTE_D4, NOTE_G3, NOTE_AS3, NOTE_C4, NOTE_D4
};

int gotDurations[] = {
    2, 2, 4, 4, 2, 2, 4, 4,
    2, 2, 4, 4, 2, 2, 4, 4,

    2, 2, 4, 4, 2, 2, 4, 4,
    2, 2, 4, 4, 2, 2, 4, 4,

    2, 2, 4, 4, 2,

    2, 4, 4, 2, 2, 4, 4, 2,
    2, 4, 4, 2,

    2, 4, 4, 1,

    1, 1, 4, 4, 1, 1, 4, 4, 2,

    4, 4, 2, 2, 4, 4, 2, 2,
    4, 4, 2, 2,

    1, 1, 4, 4, 1, 1, 4, 4, 2,

    2, 4, 4, 2, 2, 4, 4, 2,
    2, 4, 4, 2, 2, 4, 4, 2
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
       noteDuration = 1000 / mario_underworld_tempo[thisNote];
       tone(mario_underworld_melody[thisNote], noteDuration);

       // to distinguish the notes, set a minimum time between them.
       // the note's duration + 30% seems to work well:
       pauseBetweenNotes = noteDuration * 1.30;
       DELAY_MS(pauseBetweenNotes);

       // stop the tone playing:
       noTone();                
    }
}

void playkonamitune(void)
{	
    arraysize = sizeof(konami_melody) / sizeof(int);
	
	for (thisNote = 0; thisNote < arraysize; thisNote++)
	{

       // to calculate the note duration, take one second
       // divided by the note type.
       //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
	   // Had to down tempo to 100/150 
       noteDuration = 1500 / konaminoteDurations[thisNote];
       tone(konami_melody[thisNote], noteDuration);

       // to distinguish the notes, set a minimum time between them.
       // the note's duration + 30% seems to work well:
       pauseBetweenNotes = noteDuration + 50;
       DELAY_MS(pauseBetweenNotes);

       // stop the tone playing:
       noTone();                
    }
}

void playgameofthrones(void)
{
    arraysize = sizeof(got_melody) / sizeof(int);

    for (thisNote = 0; thisNote < arraysize; thisNote++)
    {
       // to calculate the note duration, take one second
       // divided by the note type.
       //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
       // Had to down tempo to 100/150
       noteDuration = 1000 / gotDurations[thisNote];
       tone(got_melody[thisNote], noteDuration);

       // to distinguish the notes, set a minimum time between them.
       // the note's duration + 30% seems to work well:
       pauseBetweenNotes = noteDuration + 70;
       DELAY_MS(pauseBetweenNotes);

       // stop the tone playing:
       noTone();
    }

}



#if 0
void GameOfThrones()
{
    for(int i=0; i<4; i++) //ok
    {
        tone(speakerPin, NOTE_G4);
        delay(500);
        noTone(speakerPin);
        tone(speakerPin, NOTE_C4);
        delay(500);
        noTone(speakerPin);
        tone(speakerPin, NOTE_DS4);
        delay(250);
        noTone(speakerPin);
        tone(speakerPin, NOTE_F4);
        delay(250);
        noTone(speakerPin);
    }

    for(int i=0; i<4; i++) //ok
    {
        tone(speakerPin, NOTE_G4);
        delay(500);
        noTone(speakerPin);
        tone(speakerPin, NOTE_C4);
        delay(500);
        noTone(speakerPin);
        tone(speakerPin, NOTE_E4);
        delay(250);
        noTone(speakerPin);
        tone(speakerPin, NOTE_F4);
        delay(250);
        noTone(speakerPin);
    }

    tone(speakerPin, NOTE_G4); //ok
    delay(500);
    noTone(speakerPin);
    tone(speakerPin, NOTE_C4);
    delay(500);
    tone(speakerPin, NOTE_DS4);
    delay(250);
    noTone(speakerPin);
    tone(speakerPin, NOTE_F4);
    delay(250);
    noTone(speakerPin);
    tone(speakerPin, NOTE_D4);
    delay(500);
    noTone(speakerPin);

    for(int i=0; i<3; i++)
    {
        tone(speakerPin, NOTE_G3);
        delay(500);
        noTone(speakerPin);
        tone(speakerPin, NOTE_AS3);
        delay(250);
        noTone(speakerPin);
        tone(speakerPin, NOTE_C4);
        delay(250);
        noTone(speakerPin);
        tone(speakerPin, NOTE_D4);
        delay(500);
        noTone(speakerPin);
    }//

    tone(speakerPin, NOTE_G3); //ok
    delay(500);
    noTone(speakerPin);
    tone(speakerPin, NOTE_AS3);
    delay(250);
    noTone(speakerPin);
    tone(speakerPin, NOTE_C4);
    delay(250);
    noTone(speakerPin);
    tone(speakerPin, NOTE_D4);
    delay(1000);
    noTone(speakerPin);

    tone(speakerPin, NOTE_F4); //ok
    delay(1000);
    noTone(speakerPin);
    tone(speakerPin, NOTE_AS3);
    delay(1000);
    noTone(speakerPin);
    tone(speakerPin, NOTE_DS4);
    delay(250);
    noTone(speakerPin);
    tone(speakerPin, NOTE_D4);
    delay(250);
    noTone(speakerPin);

    tone(speakerPin, NOTE_F4); //ok
    delay(1000);
    noTone(speakerPin);
    tone(speakerPin, NOTE_AS3);
    delay(1000);
    noTone(speakerPin);
    tone(speakerPin, NOTE_DS4);
    delay(250);
    noTone(speakerPin);
    tone(speakerPin, NOTE_D4);
    delay(250);
    noTone(speakerPin);
    tone(speakerPin, NOTE_C4);
    delay(500);
    noTone(speakerPin);

    for(int i=0; i<3; i++)
    {
        tone(speakerPin, NOTE_GS3); //ok
        delay(250);
        noTone(speakerPin);
        tone(speakerPin, NOTE_AS3);
        delay(250);
        noTone(speakerPin);
        tone(speakerPin, NOTE_C4);
        delay(500);
        noTone(speakerPin);
        tone(speakerPin, NOTE_F3);
        delay(500);
        noTone(speakerPin);
    }

    tone(speakerPin, NOTE_G4); //ok
    delay(1000);
    noTone(speakerPin);
    tone(speakerPin, NOTE_C4);
    delay(1000);
    noTone(speakerPin);
    tone(speakerPin, NOTE_DS4);
    delay(250);
    noTone(speakerPin);
    tone(speakerPin, NOTE_F4);
    delay(250);
    noTone(speakerPin);
    tone(speakerPin, NOTE_G4);
    delay(1000);
    noTone(speakerPin);
    tone(speakerPin, NOTE_C4);
    delay(1000);
    noTone(speakerPin);
    tone(speakerPin, NOTE_DS4);
    delay(250);
    noTone(speakerPin);
    tone(speakerPin, NOTE_F4);
    delay(250);
    noTone(speakerPin);
    tone(speakerPin, NOTE_D4);
    delay(500);
    noTone(speakerPin);

    for(int i=0; i<4; i++)
    {
        tone(speakerPin, NOTE_G3);
        delay(500);
        noTone(speakerPin);
        tone(speakerPin, NOTE_AS3);
        delay(250);
        noTone(speakerPin);
        tone(speakerPin, NOTE_C4);
        delay(250);
        noTone(speakerPin);
        tone(speakerPin, NOTE_D4);
        delay(500);
        noTone(speakerPin);
    }
}
#endif





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
	
