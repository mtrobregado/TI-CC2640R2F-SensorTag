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
#ifndef SENSORTAG_PLAYTUNE_H
#define SENSORTAG_PLAYTUNE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * FUNCTIONS
 */
 
 extern void playbirthdaytune(void);
 extern void playmariotune(void);
 extern void playmariounderworldtune(void);
 extern void playkonamitune(void);
 extern void playgameofthrones(void);


#ifdef __cplusplus
}
#endif

#endif /* SENSORTAG_PLAYTUNE_H */
