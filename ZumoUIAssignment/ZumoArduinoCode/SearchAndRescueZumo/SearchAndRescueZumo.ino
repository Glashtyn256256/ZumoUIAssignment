#include <Zumo32U4.h>
#include "TurnSensor.h"

//Serial1 communicates over XBee
//Serial communicates over USB cable

Zumo32U4Encoders encoders;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;
Zumo32U4LCD lcd;
Zumo32U4Buzzer buzzer;
L3G gyro;

#define NUM_SENSORS 5
#define NUM_ENCODERS 4
uint16_t lineSensorValues[NUM_SENSORS];
uint16_t motorEncoderValues[NUM_ENCODERS];
const uint16_t sensorThreshold = 200;
uint8_t leftValue;
uint8_t rightValue;
#define QTR_THRESHOLD     1000  // microseconds

// These might need to be tuned for different motor types.
#define REVERSE_SPEED     100  // 0 is stopped, 400 is full speed
#define TURN_SPEED        100
#define FORWARD_SPEED     100
#define REVERSE_DURATION  400  // ms
#define TURN_DURATION     100  // ms

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  Serial.begin(9600);
  lineSensors.initThreeSensors();
  proxSensors.initFrontSensor();
  
  turnSensorSetup();
  delay(500);
  turnSensorReset();
}
void printReadingsToSerial()
{
  char buffer[80];
  sprintf(buffer, "%4d %4d %4d %4d %4d %c\n",
    lineSensorValues[0],
    lineSensorValues[1],
    lineSensorValues[2],
    lineSensorValues[3],
    lineSensorValues[4]
  );
  Serial.print(buffer);
}

bool aboveLine(uint8_t sensorIndex)
{
  return lineSensorValues[sensorIndex] > sensorThreshold;
}

char incomingByte;
void loop() {
  // put your main code here, to run repeatedly:
incomingByte = Serial1.read();
lineSensors.read(lineSensorValues);
turnSensorUpdate();
            int angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
            //delay(2);
            motors.setSpeeds(FORWARD_SPEED + (angle * 10), FORWARD_SPEED - (angle * 10));
            
//reverse back out and then adjust the encoders to match each other, to keep it straight?
switch(incomingByte)
{
  case'w':  
            break;
  
  case 'a': motors.setLeftSpeed(-100);
            motors.setRightSpeed(100);
            while((int32_t)turnAngle < turnAngle45 * 2)
            {
              turnSensorUpdate();
            }
            motors.setLeftSpeed(0);
            motors.setRightSpeed(0);
            //reset encoders, we only want the encoders for when it's going straight.
            encoders.getCountsAndResetLeft();
            encoders.getCountsAndResetRight();
            turnSensorReset();
            break;

case's':  motors.setLeftSpeed(-100);
            motors.setRightSpeed(-100);
            turnSensorUpdate();
            //delay(2);
            break;

 case 'd': motors.setLeftSpeed(100);
            motors.setRightSpeed(-100);
           while((int32_t)turnAngle < -turnAngle45 * 2)
            {
              turnSensorUpdate();
            }
            motors.setLeftSpeed(0);
            motors.setRightSpeed(0);
            turnSensorReset();
            encoders.getCountsAndResetLeft();
            encoders.getCountsAndResetRight();
            break;
            
  case'e':  motors.setLeftSpeed(0);
            motors.setRightSpeed(0);
            Serial.print("Stopped");
            break;
  
  
  case'z':  break;
  
 

  case'x': break;
  }
}
void ScanRoom()
{
  bool objectSeen = false;
  motors.setLeftSpeed(-100);
  motors.setRightSpeed(100);
  while((int32_t)turnAngle < turnAngle45 * 2)
   {
    if(objectSeen == false)
    {
      proxSensors.read();
      leftValue = proxSensors.countsFrontWithLeftLeds();
      rightValue = proxSensors.countsFrontWithRightLeds();    
      objectSeen = leftValue >= sensorThreshold || rightValue >= sensorThreshold;
    }
    turnSensorUpdate();
   }
   
    while((int32_t)turnAngle < -turnAngle45 * 6)
    {
      if(objectSeen == false)
      {
        proxSensors.read();
        leftValue = proxSensors.countsFrontWithLeftLeds();
        rightValue = proxSensors.countsFrontWithRightLeds();    
        objectSeen = leftValue >= sensorThreshold || rightValue >= sensorThreshold;
      }
      turnSensorUpdate();
    }
}

void StraightenZumoUsingEncoders()
{
  if(encoders.getCountsLeft() < encoders.getCountsRight())
  {
    //May be some speed maths we can do to setleftspeed and add it into while loop
    //Like how the gryo stays straight
    motors.setLeftSpeed(5);
    motors.setRightSpeed(0);
    while(encoders.getCountsLeft() >= encoders.getCountsRight())
    {
      Serial.print(encoders.getCountsLeft());
      Serial.print(encoders.getCountsRight());
    } 
      motors.setLeftSpeed(0);
  }
  else
  {
    motors.setLeftSpeed(0);
    motors.setRightSpeed(5);
    while(encoders.getCountsRight() >= encoders.getCountsLeft())
    {
      Serial.print(encoders.getCountsRight());
      Serial.print(encoders.getCountsLeft());
    } 
      motors.setRightSpeed(0);
  }
  turnSensorReset();
}


  
