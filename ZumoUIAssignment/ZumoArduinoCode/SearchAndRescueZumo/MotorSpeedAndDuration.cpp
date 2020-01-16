/* MotorSpeedAndDuration.h and MotorSpeedAndDuration.cpp provides
the variable speeds for the motors, sets the time limit for any delays 
and the provides functions that set the motorspeeds.*/

#include <Wire.h>
#include "MotorSpeedAndDuration.h"

void ManualForwardSpeed()
{
  motors.setSpeeds(MANUAL_FORWARD_SPEED, MANUAL_FORWARD_SPEED);
}
void ManualReverseSpeed(){
  motors.setSpeeds(MANUAL_REVERSE_SPEED, MANUAL_REVERSE_SPEED);
}
void ManualTurnLeftSpeed(){
  motors.setSpeeds(-MANUAL_TURN_SPEED, MANUAL_TURN_SPEED);
}
void ManualTurnRightSpeed(){
 motors.setSpeeds(MANUAL_TURN_SPEED, -MANUAL_TURN_SPEED);
}

void AutomatedForwardSpeed(){
  motors.setSpeeds(AUTO_FORWARD_SPEED, AUTO_FORWARD_SPEED);
}
void AutomatedReverseSpeed(){
  motors.setSpeeds(AUTO_REVERSE_SPEED, AUTO_REVERSE_SPEED);
}
void AutomatedTurnLeftSpeed(){
   motors.setSpeeds(-AUTO_TURN_SPEED, AUTO_TURN_SPEED);
}
void AutomatedTurnRightSpeed(){
   motors.setSpeeds(AUTO_TURN_SPEED, -AUTO_TURN_SPEED);
}

void AdjustForwardSpeed(){
  motors.setSpeeds(ADJUST_FORWARD_SPEED, ADJUST_FORWARD_SPEED);
}
void AdjustReverseSpeed(){
  motors.setSpeeds(-ADJUST_REVERSE_SPEED, -ADJUST_REVERSE_SPEED);
}
void AdjustTurnLeftSpeed(){
  motors.setSpeeds(-ADJUST_TURN_SPEED, ADJUST_TURN_SPEED);
}
void AdjustTurnRightSpeed(){
  motors.setSpeeds(ADJUST_TURN_SPEED, -ADJUST_TURN_SPEED);
}

void SpeedStop(){
  motors.setSpeeds(SPEED_STOP, SPEED_STOP);
}

void SetSpeedValues(int leftvalue, int rightvalue){
   motors.setSpeeds(leftvalue, rightvalue);
}
