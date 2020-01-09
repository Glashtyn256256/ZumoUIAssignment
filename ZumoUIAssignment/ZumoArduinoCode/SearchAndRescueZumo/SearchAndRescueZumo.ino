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

#define NUM_SENSORS 3
#define NUM_ENCODERS 4
uint16_t lineSensorValues[NUM_SENSORS];
uint16_t motorEncoderValues[NUM_ENCODERS];
const uint16_t sensorThreshold = 200;
const uint16_t sensorProximityThreshold = 5;
uint16_t leftValue;
uint16_t rightValue;
#define QTR_THRESHOLD     1200  // microseconds

// These might need to be tuned for different motor types.
#define REVERSE_SPEED     100  // 0 is stopped, 400 is full speed
#define TURN_SPEED        100
#define FORWARD_SPEED     100
#define REVERSE_DURATION  250  // ms
#define TURN_DURATION     100  // ms
#define MAIN_SPEED 100
#define Kp 1
#define STRAIGHTFACTOR 1  // Adjust this to correct for minor curve.  Should be in the 0.9 to 1.1 range


void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  Serial.begin(9600);

  lineSensors.initThreeSensors();
  proxSensors.initFrontSensor();
  buttonA.waitForButton();
  //turnSensorSetup();
  //lineSensorCalibrateSetup();
  Serial.print(readBatteryMillivolts());

}
void printReadingsToSerial()
{
  char buffer[80];
  sprintf(buffer, "%4d %4d %4d %4d %4d %c\n",
          lineSensorValues[0],
          lineSensorValues[1],
          lineSensorValues[2]
          //lineSensorValues[3],
          //lineSensorValues[4]
         );
  Serial.print(buffer);
}
void printReadingsToSerials()
{
  char buffer[80];
  sprintf(buffer, "%1d %1d %c\n",
          leftValue, rightValue
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
  //reverse back out and then adjust the encoders to match each other, to keep it straight?
  switch (incomingByte)
  {
    case'w':
      MovementGoingForward();
      break;

    case 'a':
      MotorSpeedTurnLeft();
      int error;
      int correction;
      int currentSpeedLeft = MAIN_SPEED;
      int currentSpeedRight = MAIN_SPEED;

      int countsLeft = encoders.getCountsAndResetLeft();
      int countsRight = encoders.getCountsAndResetRight();

      while ((countsLeft < 648) && (countsRight < 648))
      {
        countsLeft = -encoders.getCountsLeft();
        countsRight = encoders.getCountsRight();
        error = countsLeft - STRAIGHTFACTOR * countsRight;
        correction = Kp * error;
        currentSpeedRight = MAIN_SPEED + correction;
        motors.setRightSpeed(currentSpeedRight);
      }
      if ((countsLeft < 648))
      {
        while ((countsLeft < 648))
        {
          countsLeft = -encoders.getCountsLeft();
        countsRight = encoders.getCountsRight();
          motors.setRightSpeed(0);

        }
      }
      else if (countsRight < 648)
      {
        while ((countsRight < 648))
        {
          countsLeft = -encoders.getCountsLeft();
        countsRight = encoders.getCountsRight();
          motors.setLeftSpeed(0);

        }
      }
      motors.setLeftSpeed(0);
      motors.setRightSpeed(0);
      //reset encoders, we only want the encoders for when it's going straight.
      Serial1.println(encoders.getCountsAndResetLeft());
      Serial1.println(encoders.getCountsAndResetRight());
      turnSensorReset();
      break;

    case's':  motors.setLeftSpeed(-100);
      motors.setRightSpeed(-100);
      turnSensorUpdate();
      //delay(2);
      break;

    case 'd': MotorSpeedTurnRight();
      Serial.print("working");
      while ((int32_t)turnAngle > -turnAngle45 * 2)
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


    case'z':  ScanRoom();
      break;



    case'x': break;
  }
}
void ScanRoom()
{
  bool objectSeen = false;
  objectSeen = ScanRoomProximityTurnLeft(objectSeen);
  delay(100);
  objectSeen = ScanRoomProximityTurnRight(objectSeen);
  delay(100);
  objectSeen = ScanRoomProximityTurnRight(objectSeen);
  delay(100);
  objectSeen = ScanRoomProximityTurnRight(objectSeen);
  delay(100);
  if (objectSeen) {
    Serial.print("TRUE");
  } else {
    Serial.print("FALSE");
  }
}

bool ScanRoomProximityTurnLeft(bool objectseen)
{
  MotorSpeedTurnLeft();
  while ((int32_t)turnAngle < turnAngle45 * 2)
  {
    if (objectseen == false)
    {
      proxSensors.read();
      leftValue = proxSensors.countsFrontWithLeftLeds();
      rightValue = proxSensors.countsFrontWithRightLeds();
      printReadingsToSerials();
      objectseen = leftValue >= sensorProximityThreshold || rightValue >= sensorProximityThreshold;
    }
    turnSensorUpdate();
  }
  turnSensorReset();
  MotorSpeedStop();
  return objectseen;
}

bool ScanRoomProximityTurnRight(bool objectseen)
{
  MotorSpeedTurnRight();
  while ((int32_t)turnAngle > -turnAngle45 * 2)
  {
    if (objectseen == false)
    {
      proxSensors.read();
      leftValue = proxSensors.countsFrontWithLeftLeds();
      rightValue = proxSensors.countsFrontWithRightLeds();
      printReadingsToSerials();
      objectseen = leftValue >= sensorProximityThreshold || rightValue >= sensorProximityThreshold;
    }
    turnSensorUpdate();
  }
  turnSensorReset();
  MotorSpeedStop();
  return objectseen;
}

void StraightenZumoUsingEncoders()
{
  if (encoders.getCountsLeft() < encoders.getCountsRight())
  {
    //May be some speed maths we can do to setleftspeed and add it into while loop
    //Like how the gryo stays straight
    motors.setLeftSpeed(5);
    motors.setRightSpeed(0);
    while (encoders.getCountsLeft() >= encoders.getCountsRight())
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
    while (encoders.getCountsRight() >= encoders.getCountsLeft())
    {
      Serial.print(encoders.getCountsRight());
      Serial.print(encoders.getCountsLeft());
    }
    motors.setRightSpeed(0);
  }
  turnSensorReset();
}

void MotorSpeedTurnLeft()
{
  motors.setLeftSpeed(-MAIN_SPEED);
  motors.setRightSpeed(MAIN_SPEED);
}
void MotorSpeedTurnRight()
{
  motors.setLeftSpeed(MAIN_SPEED);
  motors.setRightSpeed(-MAIN_SPEED);
}
void MotorSpeedForward()
{
  motors.setLeftSpeed(MAIN_SPEED);
  motors.setRightSpeed(MAIN_SPEED);
}
void MotorSpeedBackward()
{
  motors.setLeftSpeed(-MAIN_SPEED);
  motors.setRightSpeed(-MAIN_SPEED);
}
void MotorSpeedStop()
{
  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);
}

static void lineSensorCalibrateSetup()
{
  lcd.clear();

  // Delay so the robot does not move while the user is still
  // touching the button.
  delay(1000);

  // We use the gyro to turn so that we don't turn more than
  // necessary, and so that if there are issues with the gyro
  // then you will know before actually starting the robot.

  turnSensorReset();

  // Turn to the left 90 degrees.
  motors.setSpeeds(-FORWARD_SPEED, FORWARD_SPEED);
  while ((int32_t)turnAngle < turnAngle45 * 2)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }

  // Turn to the right 90 degrees.
  motors.setSpeeds(FORWARD_SPEED, -FORWARD_SPEED);
  while ((int32_t)turnAngle > -turnAngle45 * 2)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }

  // Turn back to center using the gyro.
  motors.setSpeeds(-FORWARD_SPEED, FORWARD_SPEED);
  while ((int32_t)turnAngle < 0)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }

  // Stop the motors.
  motors.setSpeeds(0, 0);
}

void MovementGoingForward()
{
  turnSensorReset();
  int error;
  int correction;
  int currentSpeedLeft = MAIN_SPEED;
  int currentSpeedRight = MAIN_SPEED;
  int countsLeft = encoders.getCountsAndResetLeft();
  int countsRight = encoders.getCountsAndResetRight();

  motors.setSpeeds(MAIN_SPEED, MAIN_SPEED);
  while (true)
  {

    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsLeft - STRAIGHTFACTOR * countsRight;
    correction = Kp * error;
    currentSpeedRight = MAIN_SPEED + correction;
    motors.setSpeeds(currentSpeedLeft, currentSpeedRight);
    turnSensorUpdate();
    lineSensors.read(lineSensorValues);
    if (lineSensorValues[2] > QTR_THRESHOLD || lineSensorValues[0] > QTR_THRESHOLD)
    {
      printReadingsToSerial();
      delay(100);
      lineSensors.read(lineSensorValues);
      printReadingsToSerial();
      if ((lineSensorValues[0] > QTR_THRESHOLD
           && lineSensorValues[1] > QTR_THRESHOLD)
          || (lineSensorValues[2] > QTR_THRESHOLD
              && lineSensorValues[1] > QTR_THRESHOLD))
      {
        motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
        delay(200);
        motors.setLeftSpeed(0);
        motors.setRightSpeed(0);
        break;
      }
      else
      {
        motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
        delay(REVERSE_DURATION);
        motors.setLeftSpeed(0);
        motors.setRightSpeed(0);

        if (lineSensorValues[2] > QTR_THRESHOLD)
        {
          motors.setSpeeds(-REVERSE_SPEED, REVERSE_SPEED);
          delay(50);
          motors.setLeftSpeed(0);
          motors.setRightSpeed(0);
        }
        else
        {
          motors.setSpeeds(REVERSE_SPEED, -REVERSE_SPEED);
          delay(50);
          motors.setLeftSpeed(0);
          motors.setRightSpeed(0);
        }
        //        if ((int32_t)turnAngle < 0)
        //        {
        //          MotorSpeedTurnLeft();
        //          while ((int32_t)turnAngle < 0)
        //          {
        //            turnSensorUpdate();
        //          }
        //          MotorSpeedStop();
        //        }
        //        else if ((int32_t)turnAngle > 0)
        //        {
        //          MotorSpeedTurnRight();
        //          while ((int32_t)turnAngle < 0)
        //          {
        //            turnSensorUpdate();
        //          }
        //          MotorSpeedStop();
        //        }
      }
      printReadingsToSerial();
      countsLeft = encoders.getCountsAndResetLeft();
      countsRight = encoders.getCountsAndResetRight();
    }
  }
}
