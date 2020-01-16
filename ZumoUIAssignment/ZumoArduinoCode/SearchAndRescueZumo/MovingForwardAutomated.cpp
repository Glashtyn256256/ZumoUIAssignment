/* BuzzerAndLED.h and BuzzerAndLED.cpp contains the functions
   that allow our buzzer to play a tune and allows us to have
   turn on or off our LED lights on top of making them flash

   I was able to undertand how to use the LED lights by following 
   the example BlinkLED for the Zumo32u4 and I looked at the class reference for 
   the buzzer it is also where I got my tune from. 
   https://pololu.github.io/zumo-32u4-arduino-library/class_zumo32_u4_buzzer.html#ab72bde97ceceef8705f1bbaeccb970db
*/

#include <Wire.h>
#include "BuzzerAndLED.h"

void PlayBuzzer()
{
    buzzer.play("!L16 V8 cdefgab>cbagfedc");
}

void TurnOffLED()
{
  ledRed(0);
  ledYellow(0);
  ledGreen(0);
}
void TurnOnLED()
{
  ledRed(1);
  ledYellow(1);
  ledGreen(1);
}
void FlashLED()
{
  TurnOffLED();
  delay(FLASH_DURATION);
  for (int i = 0; i < 10; i++)
  {
    TurnOnLED();
    delay(FLASH_DURATION);
    TurnOffLED();
  }
  delay(FLASH_DURATION);
  TurnOnLED();
}
