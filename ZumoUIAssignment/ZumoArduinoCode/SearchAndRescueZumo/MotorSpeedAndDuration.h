/* MotorSpeedAndDuration.h and MotorSpeedAndDuration.cpp provides
the variable speeds for the motors, sets the time limit for any delays 
and provides functions that set the motorspeeds.*/

#pragma once

#include <Zumo32U4.h>

/* These defines are what the speeds will be for the manual and
  automated speed functions. I use define instead of ints, it saves 
  us space by using no memory and the value can't be changed. If we used 
  a int then it take a small amount of memory.
  
  We have serpate speeds for both the manual and the automated we don't
  want manual being to fast since it's easy to overshoot or not stop at 
  the right time since it's controlled by the user. Automated can be faster 
  since it uses linesensors and has more precision when stopping which is why 
  the speeds are higher.
 */

#define MANUAL_FORWARD_SPEED 100
#define MANUAL_REVERSE_SPEED 100
#define MANUAL_TURN_SPEED 100

#define AUTO_FORWARD_SPEED 150
#define AUTO_REVERSE_SPEED 150
#define AUTO_TURN_SPEED    200

#define ADJUST_FORWARD_SPEED 200
#define ADJUST_REVERSE_SPEED 200
#define ADJUST_TURN_SPEED 200

#define SPEED_STOP 0

//How long I want the delay to be when the Zumo has ran into the wall.
#define REVERSE_DURATION  250  // ms

//How long I want the zumo to turn when it needs to correct it's position after it has hit a wall.
#define TURN_DURATION     60  // ms

//How long I want the motor speeds to adjust the zumos position, before stopping, 
#define ADJUST_DURATION 10 //ms

//Sets the speeds of the Motors when using Manual controls
void ManualForwardSpeed();
void ManualReverseSpeed();
void ManualTurnLeftSpeed();
void ManualTurnRightSpeed();

//Sets the speeds of the Motors when using Auto Controls
void AutomatedForwardSpeed();
void AutomatedReverseSpeed();
void AutomatedTurnLeftSpeed();
void AutomatedTurnRightSpeed();


void AdjustForwardSpeed();
void AdjustReverseSpeed();
void AdjustTurnLeftSpeed();
void AdjustTurnRightSpeed();
//Sets the speeds for when we want to stop.
void SpeedStop();

/*Set the speeds from the two values that are passed through. 
  This will be used when we work out the correction that's needed
  for the motors
*/ 
void SetSpeedValues(int leftspeed,int rightspeed);

void SetSpeedValuesDurationAndStop(int leftvalue, int rightvalue, int duration);

//Objects defined below must be in the sketch.

extern Zumo32U4Motors motors;
