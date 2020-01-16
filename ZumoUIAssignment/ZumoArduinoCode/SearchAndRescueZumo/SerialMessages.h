/* BuzzerAndLED.h and BuzzerAndLED.cpp contains the functions
   that allow our buzzer to play a tune and allows us to have
   turn on or off our LED lights on top of making them flash

   I was able to undertand how to use the LED lights by following 
   the example BlinkLED and I looked at the class reference for 
   the buzzer it is also where I got my tune from. 
   https://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_buzzer.html#ab72bde97ceceef8705f1bbaeccb970db
*/

#pragma once

#include <Zumo32U4.h>

#define FLASH_DURATION 100 //ms

//We call this when we want a tune to be played for us on the buzzer.
void PlayBuzzer();

/* We call these functions to either turn off the LED lights, turn 
on the LED lights or make the LED lights flash.*/
void TurnOffLED();
void TurnOnLED();
void FlashLED();

extern Zumo32U4Buzzer buzzer;
