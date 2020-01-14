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
#define QTR_THRESHOLD     500  // microseconds
#define ENCODER_TURN_LIMIT 658
// These might need to be tuned for different motor types.
#define REVERSE_SPEED     150  // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     150
#define REVERSE_DURATION  250  // ms
#define TURN_DURATION     60  // ms
#define MAIN_SPEED 150
#define Kp 1
#define STRAIGHTFACTOR 1.015  // Adjust this to correct for minor curve.  Should be in the 0.9 to 1.1 range
#define DEGREES 90
#define TJUNCTION 0
int rightDistanceEncoderArray[20];
int leftDistanceEncoderArray[20];
int totalDistanceValueRightEncoder;
int totalDistanceValueLeftEncoder;

bool RoomHasCivilian[20];
bool CorridorHasRoom[20];

char movementArray[20];
int indexPositionDistance;
int indexPositionMovement;
int indexPositionCorridorRooms;

int AmountOfRooms = 1;
bool ReturnHomeIsTrue = false;
bool SurvivorFollowing = false;


void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  Serial.begin(9600);
  InitArrays();
  lineSensors.initThreeSensors();
  proxSensors.initFrontSensor();
  turnSensorSetup();
  lineSensorCalibrateSetup();
  Serial1.print("Battery Power: ");
  Serial1.println(readBatteryMillivolts());

}

void PlayBuzzer()
{
  if (SurvivorFollowing)
  {
    buzzer.play("!L16 V8 cdefgab>cbagfedc");
  }
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
  delay(100);
  for(int i = 0; i < 10; i++)
  {
    TurnOnLED();
    delay(100);
    TurnOffLED();
  }
  delay(100);
  TurnOnLED();
}
void printReadingsToSerial()
{
  char buffer[80];
  sprintf(buffer, "%4d %4d %4d %c\n",
          lineSensorValues[0],
          lineSensorValues[1],
          lineSensorValues[2]
         );
  Serial.print(buffer);
}

void printReadingsToSerials()
{
  char buffer[80];
  sprintf(buffer, "%1d %1d %c\n",
          leftValue, rightValue
         );
  Serial1.print(buffer);
}

bool aboveLine(uint8_t sensorIndex)
{
  return lineSensorValues[sensorIndex] > sensorThreshold;
}

void TurnRightUsingEncoders()
{
  MotorSpeedTurnRight();
  int error;
  int correction;
  int currentSpeedLeft = TURN_SPEED;
  int currentSpeedRight = TURN_SPEED;

  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  int countsLeft =  encoders.getCountsLeft();
  int countsRight = encoders.getCountsRight();
  while ((countsRight < ENCODER_TURN_LIMIT) && (countsLeft < ENCODER_TURN_LIMIT))
  {
    countsLeft = encoders.getCountsLeft();
    countsRight = -encoders.getCountsRight();
    error = countsRight - STRAIGHTFACTOR * countsLeft;
    correction = Kp * error;
    currentSpeedLeft = TURN_SPEED + correction;
    motors.setLeftSpeed(currentSpeedLeft);
  }
  //   if (countsRight < ENCODER_TURN_LIMIT)
  //  {
  //    while ((countsRight < ENCODER_TURN_LIMIT))
  //    {
  //      countsLeft = encoders.getCountsLeft();
  //      countsRight = -encoders.getCountsRight();
  //      motors.setLeftSpeed(-100);
  //
  //    }
  //  }
  //  else if ((countsLeft < ENCODER_TURN_LIMIT))
  //  {
  //    while ((countsLeft < ENCODER_TURN_LIMIT))
  //    {
  //      countsLeft = encoders.getCountsLeft();
  //      countsRight = -encoders.getCountsRight();
  //      motors.setRightSpeed(100);
  //
  //    }
  //  }
  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);
  //reset encoders, we only want the encoders for when it's going straight.
  //Serial1.println(encoders.getCountsAndResetLeft());
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  //Serial1.println(encoders.getCountsAndResetRight());
  turnSensorReset();
}

void TurnLeftUsingEncoders()
{
  MotorSpeedTurnLeft();
  int error;
  int correction;
  int currentSpeedLeft = TURN_SPEED;
  int currentSpeedRight = TURN_SPEED;

  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  int countsLeft =  encoders.getCountsLeft();
  int countsRight = encoders.getCountsRight();

  while ((countsLeft < ENCODER_TURN_LIMIT) && (countsRight < ENCODER_TURN_LIMIT))
  {

    countsLeft = -encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsLeft - STRAIGHTFACTOR * countsRight;
    correction = Kp * error;
    currentSpeedRight = TURN_SPEED + correction;
    motors.setRightSpeed(currentSpeedRight);
  }
  //      if ((countsLeft < ENCODER_TURN_LIMIT))
  //      {
  //        while ((countsLeft < ENCODER_TURN_LIMIT))
  //        {
  //          countsLeft = -encoders.getCountsLeft();
  //          countsRight = encoders.getCountsRight();
  //          motors.setRightSpeed(-100);
  //
  //        }
  //      }
  //      else if (countsRight < ENCODER_TURN_LIMIT)
  //      {
  //        while ((countsRight < ENCODER_TURN_LIMIT))
  //        {
  //          countsLeft = -encoders.getCountsLeft();
  //          countsRight = encoders.getCountsRight();
  //          motors.setLeftSpeed(100);
  //
  //        }
  //      }
  motors.setLeftSpeed(0);
  motors.setRightSpeed(0);
  //reset encoders, we only want the encoders for when it's going straight.
  Serial1.println(encoders.getCountsAndResetLeft());
  Serial1.println(encoders.getCountsAndResetRight());
  turnSensorReset();
}

char incomingByte;
void loop() {
  // put your main code here, to run repeatedly:
  incomingByte = Serial1.read();
  //reverse back out and then adjust the encoders to match each other, to keep it straight?
  switch (incomingByte)
  {
    case'w':
      Serial1.println("Manual: Moving Forward");
      MotorSpeedForward();
      Serial1.flush();
      break;

    case 'a':
      Serial1.println("Manual: Turning left");
      MotorSpeedTurnLeft();
      Serial1.flush();
      break;

    case 'd':
      Serial1.println("Manual: Turning right");
      MotorSpeedTurnRight();
      Serial1.flush();
      break;

    case 's':
      Serial1.println("Manual: Moving Backwards");
      MotorSpeedBackward();
      Serial1.flush();
      break;

    case't':
      Serial1.println("Adjust: Moving Forward");
      MotorSpeedForward();
      delay(10);
      MotorSpeedStop();
      Serial1.flush();
      break;

    case 'f':
      Serial1.println("Adjust: Turning Left");
      MotorSpeedTurnLeft();
      delay(10);
      MotorSpeedStop();
      turnSensorReset();
      Serial1.flush();
      break;

    case 'y':
      Serial1.println("Adjust: Turning Right");
      MotorSpeedTurnRight();
      delay(10);
      MotorSpeedStop();
      turnSensorReset();
      Serial1.flush();
      break;

    case 'g':
      Serial1.println("Adjust: Moving Backwards");
      MotorSpeedBackward();
      delay(10);
      MotorSpeedStop();
      Serial1.flush();
      break;

    case 'e':
      MotorSpeedStop();
      turnSensorReset();
      Serial1.println("Movement Stopped");
      Serial1.flush();
      break;

    case 'q':
      Serial1.println("Manual: Turning Left 90 Degrees");
      TurnLeft(90);
      Serial1.println("Manual: Turning Left Completed, Press C To Move Forward");
      Serial1.flush();
      break;

    case 'r':
      Serial1.println("Manual: Turning Right 90 Degrees");
      TurnRight(90);
      Serial1.println("Manual: Turning Right Completed, Press C To Move Forward");
      Serial1.flush();
      break;

    case'i': case'c':
      Serial1.println("Automated: Moving Forward");
      ResetEncoderTotalValues();
      MovementGoingForward();
      Serial1.flush();
      break;

    case 'j':
      Serial1.println("Automated: Turning Left 90 Degrees");
      TurnLeft(90);
      Serial1.println("Automated: Turning Left Completed");
      AddMovementValueIntoArray('d');
      delay(100);
      Serial1.println("Automated: Moving Forward");
      ResetEncoderTotalValues();
      MovementGoingForward();
      Serial1.flush();
      break;

    case 'l':
      Serial1.println("Automated: Turning Right 90 Degrees");
      TurnRight(90);
      Serial1.println("Automated: Turning Right Completed");
      AddMovementValueIntoArray('a');
      delay(100);
      Serial1.println("Automated: Moving Forward");
      ResetEncoderTotalValues();
      MovementGoingForward();
      Serial1.flush();
      break;

    case 'b':
      Serial1.println("Automated: Turning Right 180 Degrees");
      delay(200);
      TurnLeft(90);
      delay(200);
      TurnLeft(90);
      delay(200);
      ResetEncoderTotalValues();

      if (CorridorHasRoom[indexPositionCorridorRooms - 2])
      {
        totalDistanceValueRightEncoder += rightDistanceEncoderArray[indexPositionDistance - 1];
        totalDistanceValueRightEncoder += rightDistanceEncoderArray[indexPositionDistance - 2];
        totalDistanceValueLeftEncoder += leftDistanceEncoderArray[indexPositionDistance - 1];
        totalDistanceValueLeftEncoder += leftDistanceEncoderArray[indexPositionDistance - 2];
        Serial1.println("Adds all four if has room in corrdior");
      }
      else
      {
        totalDistanceValueRightEncoder += rightDistanceEncoderArray[indexPositionDistance - 1];
        totalDistanceValueLeftEncoder += leftDistanceEncoderArray[indexPositionDistance - 1];
        Serial1.println("Adds only two if room in corrdior");
        Serial1.println(totalDistanceValueRightEncoder);
        Serial1.println(totalDistanceValueLeftEncoder);
      }
      AddEncoderValuesIntoArray();
      AddMovementValueIntoArray('b');
      Serial1.println("Automated: Moving Forward");
      MovementForwardUsingDistance();
      Serial1.println("Automated: We've past the T junction");
      MotorSpeedStop();
      //Here we will check if(position is equal to x or z, if it isn't then have another if.
      // saying if position is equal to n then delete size 5, if it isn't then delete two. means it's not got a room.
            if (movementArray[indexPositionMovement - 2] != 'z' || movementArray[indexPositionMovement - 2] != 'x')
            {
              if (movementArray[indexPositionMovement - 2] == 'n')
              { 
                if (movementArray[indexPositionMovement - 3] == 'a')
                {
                  movementArray[indexPositionMovement - 3] == 'd';
                }
                else
                {
                  movementArray[indexPositionMovement - 3] == 'a';
                }
                indexPositionDistance -= 3;
                indexPositionMovement -= 2;
               
              else
              {
                if (movementArray[indexPositionMovement - 2] == 'a')
                {
                  movementArray[indexPositionMovement - 2] == 'd';
                }
                else
                {
                  movementArray[indexPositionMovement - 2] == 'a';
                }
                indexPositionDistance -= 2;
                indexPositionMovement -= 1;
              }
            }
      //Set to zero
      ResetEncoderTotalValues();
      //Add the 200 distance to the encoder values since we used that much to pass the t junction
      //
      AddEncoderValues(TJUNCTION, TJUNCTION);
      MotorSpeedStop();
      Serial1.flush();
      //MovementGoingForward();
      break;


    case 'h':
      ReturnHomeIsTrue = true;
      //We decrement here to get the last position in array with our values.
      indexPositionMovement--;
      indexPositionDistance--;
      TurnLeft(90);
      TurnLeft(90);
      SwitchCaseForAutomaticBaseReturn();
      break;
  }
}
void TurnRight(int degrees) {
  turnSensorReset();
  int error;
  int correction;
  int currentSpeedLeft = TURN_SPEED;
  int currentSpeedRight = -TURN_SPEED;
  int countsLeft = encoders.getCountsAndResetLeft();
  int countsRight = encoders.getCountsAndResetRight();
  motors.setSpeeds(TURN_SPEED, -TURN_SPEED);

  int angle = 0;
  do {
    delay(1);

    countsLeft = encoders.getCountsLeft();
    countsRight = -encoders.getCountsRight();
    error = countsRight - STRAIGHTFACTOR * countsLeft;
    correction = Kp * error;
    currentSpeedLeft = TURN_SPEED + correction;
    motors.setLeftSpeed(currentSpeedLeft);
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    turnSensorUpdate();
  } while (angle > -degrees);
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  motors.setSpeeds(0, 0);
  turnSensorReset();
}

void TurnLeft(int degrees) {
  turnSensorReset();
  int error;
  int correction;
  int currentSpeedLeft = -TURN_SPEED;
  int currentSpeedRight = TURN_SPEED;
  int countsLeft = encoders.getCountsAndResetLeft();
  int countsRight = encoders.getCountsAndResetRight();

  motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
  int angle = 0;
  do {
    delay(1);

    countsLeft = -encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsLeft - STRAIGHTFACTOR * countsRight;
    correction = Kp * error;
    currentSpeedRight = TURN_SPEED + correction;
    motors.setRightSpeed(currentSpeedRight);
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    turnSensorUpdate();
  } while (angle < degrees);
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  motors.setSpeeds(0, 0);
  turnSensorReset();
}



bool ScanRoomProximityTurnLeftGyro(bool objectseen, int degrees)
{
  turnSensorReset();
  int error;
  int correction;
  int currentSpeedLeft = -TURN_SPEED;
  int currentSpeedRight = TURN_SPEED;
  int countsLeft = encoders.getCountsAndResetLeft();
  int countsRight = encoders.getCountsAndResetRight();

  MotorSpeedTurnLeft();
  int angle = 0;
  do {
    delay(1);
    countsLeft = -encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsLeft - STRAIGHTFACTOR * countsRight;
    correction = Kp * error;
    currentSpeedRight = TURN_SPEED + correction;
    motors.setRightSpeed(currentSpeedRight);
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    if (objectseen == false)
    {
      proxSensors.read();
      leftValue = proxSensors.countsFrontWithLeftLeds();
      rightValue = proxSensors.countsFrontWithRightLeds();
      // printReadingsToSerials();
      objectseen = leftValue >= sensorProximityThreshold || rightValue >= sensorProximityThreshold;
    }
    turnSensorUpdate();
  } while (angle < degrees);
  MotorSpeedStop();
  turnSensorReset();
  return objectseen;
}

bool ScanRoomProximityTurnRightGyro(bool objectseen, int degrees)
{
  turnSensorReset();
  int error;
  int correction;
  int currentSpeedLeft = TURN_SPEED;
  int currentSpeedRight = -TURN_SPEED;
  int countsLeft = encoders.getCountsAndResetLeft();
  int countsRight = encoders.getCountsAndResetRight();
  motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
  int angle = 0;
  do {
    delay(1);

    countsLeft = encoders.getCountsLeft();
    countsRight = -encoders.getCountsRight();
    error = countsRight - STRAIGHTFACTOR * countsLeft;
    correction = Kp * error;
    currentSpeedLeft = TURN_SPEED + correction;
    motors.setLeftSpeed(currentSpeedLeft);
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    if (objectseen == false)
    {
      proxSensors.read();
      leftValue = proxSensors.countsFrontWithLeftLeds();
      rightValue = proxSensors.countsFrontWithRightLeds();
      // printReadingsToSerials();
      objectseen = leftValue >= sensorProximityThreshold || rightValue >= sensorProximityThreshold;
    }
    turnSensorUpdate();
  } while (angle > -degrees);
  MotorSpeedStop();
  turnSensorReset();
  return objectseen;
}

bool ScanRoomProximityTurnRightEncoder(bool objectseen)
{
  MotorSpeedTurnRight();
  turnSensorReset();
  int error;
  int correction;
  int currentSpeedLeft = TURN_SPEED;
  int currentSpeedRight = TURN_SPEED;

  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  int countsLeft =  encoders.getCountsLeft();
  int countsRight = encoders.getCountsRight();
  do {
    delay(1);
    turnSensorUpdate();
    countsLeft =  encoders.getCountsLeft();
    countsRight = -encoders.getCountsRight();
    error = countsLeft - STRAIGHTFACTOR * countsRight;
    correction = Kp * error;
    currentSpeedRight = TURN_SPEED + correction;
    motors.setRightSpeed(currentSpeedRight);

    if (objectseen == false)
    {
      proxSensors.read();
      leftValue = proxSensors.countsFrontWithLeftLeds();
      rightValue = proxSensors.countsFrontWithRightLeds();
      // printReadingsToSerials();
      objectseen = leftValue >= sensorProximityThreshold || rightValue >= sensorProximityThreshold;
    }

  } while ((countsRight < ENCODER_TURN_LIMIT) && (countsLeft < ENCODER_TURN_LIMIT));
  MotorSpeedStop();
  turnSensorReset();
  return objectseen;
}

bool ScanRoomProximityTurnLeftEncoder(bool objectseen)
{
  MotorSpeedTurnLeft();
  turnSensorReset();
  int error;
  int correction;
  int currentSpeedLeft = TURN_SPEED;
  int currentSpeedRight = TURN_SPEED;

  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  int countsLeft =  encoders.getCountsLeft();
  int countsRight = encoders.getCountsRight();
  do {
    delay(1);
    turnSensorUpdate();
    countsLeft = -encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsLeft - STRAIGHTFACTOR * countsRight;
    correction = Kp * error;
    currentSpeedRight = TURN_SPEED + correction;
    motors.setRightSpeed(currentSpeedRight);

    if (objectseen == false)
    {
      proxSensors.read();
      leftValue = proxSensors.countsFrontWithLeftLeds();
      rightValue = proxSensors.countsFrontWithRightLeds();
      // printReadingsToSerials();
      objectseen = leftValue >= sensorProximityThreshold || rightValue >= sensorProximityThreshold;
    }
  } while ((countsLeft < ENCODER_TURN_LIMIT) && (countsRight < ENCODER_TURN_LIMIT));
  MotorSpeedStop();
  turnSensorReset();
  return objectseen;
}

void MotorSpeedTurnLeft()
{
  motors.setLeftSpeed(-TURN_SPEED);
  motors.setRightSpeed(TURN_SPEED);
}
void MotorSpeedTurnRight()
{
  motors.setLeftSpeed(TURN_SPEED);
  motors.setRightSpeed(-TURN_SPEED);
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
  turnSensorReset();
  // Turn to the right 90 degrees.
  motors.setSpeeds(-FORWARD_SPEED, FORWARD_SPEED);
  while ((int32_t)turnAngle < turnAngle45 * 2)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }

  turnSensorReset();
  motors.setSpeeds(-FORWARD_SPEED, FORWARD_SPEED);
  while ((int32_t)turnAngle < turnAngle45 * 2)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }

  turnSensorReset();
  motors.setSpeeds(-FORWARD_SPEED, FORWARD_SPEED);
  while ((int32_t)turnAngle < turnAngle45 * 2)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }

  turnSensorReset();
  // Turn back to center using the gyro.
  turnSensorReset();
  motors.setSpeeds(-FORWARD_SPEED, FORWARD_SPEED);
  while ((int32_t)turnAngle < turnAngle45 * 2)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }
  turnSensorReset();
  // Turn to the right 90 degrees.
  motors.setSpeeds(FORWARD_SPEED, -FORWARD_SPEED);
  while ((int32_t)turnAngle > -turnAngle45 * 2)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }
  turnSensorReset();
  motors.setSpeeds(FORWARD_SPEED, -FORWARD_SPEED);
  while ((int32_t)turnAngle > -turnAngle45 * 2)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }
  turnSensorReset();
  motors.setSpeeds(FORWARD_SPEED, -FORWARD_SPEED);
  while ((int32_t)turnAngle > -turnAngle45 * 2)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }
  turnSensorReset();
  motors.setSpeeds(FORWARD_SPEED, -FORWARD_SPEED);
  while ((int32_t)turnAngle > -turnAngle45 * 2)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }
  turnSensorReset();

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
    delay(2);
    incomingByte = Serial1.read();
    if (incomingByte == 'e')
    {
      MotorSpeedStop();
      SwitchCaseForSearchingRoomInMovement();
      break;
    }
    delay(2);
    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsLeft - STRAIGHTFACTOR * countsRight;
    correction = Kp * error;
    currentSpeedRight = MAIN_SPEED + correction;
    motors.setSpeeds(currentSpeedLeft, currentSpeedRight);
    lineSensors.read(lineSensorValues);
    if (lineSensorValues[2] > QTR_THRESHOLD || lineSensorValues[0] > QTR_THRESHOLD)
    {
      delay(50);
      lineSensors.read(lineSensorValues);
      printReadingsToSerial();
      if ((lineSensorValues[0] > QTR_THRESHOLD
           && lineSensorValues[1] > QTR_THRESHOLD)
          || (lineSensorValues[2] > QTR_THRESHOLD
              && lineSensorValues[1] > QTR_THRESHOLD))
      {
        motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
        delay(200);
        MotorSpeedStop();
        AddEncoderValues(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
        AddEncoderValuesIntoArray();
        Serial1.println(leftDistanceEncoderArray[indexPositionDistance - 1]);
        Serial1.println(rightDistanceEncoderArray[indexPositionDistance - 1]);
        indexPositionCorridorRooms++;
        break;
      }
      else
      {
        motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
        delay(REVERSE_DURATION);
        MotorSpeedStop();
        if (lineSensorValues[2] > QTR_THRESHOLD)
        {
          motors.setSpeeds(-REVERSE_SPEED, REVERSE_SPEED);
          delay(TURN_DURATION);
          MotorSpeedStop();
          AddEncoderValues(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
          delay(100);
          //break;
        }
        else
        {
          motors.setSpeeds(REVERSE_SPEED, -REVERSE_SPEED);
          delay(TURN_DURATION);
          MotorSpeedStop();
          AddEncoderValues(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
          delay(100);
          //break;
        }
      }
      printReadingsToSerial();
    }
  }
}

void SwitchCaseForAutomaticBaseReturn()
{
  //Do the first movement, no need to put in w command.
  MovementForwardUsingDistanceAutomated();
  while (true)
  {
    char nextMovement = movementArray[indexPositionMovement];
    switch (nextMovement)
    {
      case 'a':
        Serial1.println("turn left");
        TurnLeft(90);
        indexPositionMovement--;
        indexPositionDistance--;
        MovementForwardUsingDistanceAutomated();
        break;

      case 'd':
        Serial1.println("turn right");
        TurnRight(90);
        indexPositionMovement--;
        indexPositionDistance--;
        MovementForwardUsingDistanceAutomated();
        break;

      case 'z':
        // Serial1.println("turn left scan");
        TurnLeftScanRoom();
        indexPositionMovement--;
        indexPositionDistance--;
        MovementForwardUsingDistanceAutomated();
        break;

      case 'x':
        //  Serial1.println("turn right scan");
        TurnRightScanRoom();
        indexPositionMovement--;
        indexPositionDistance--;
        MovementForwardUsingDistanceAutomated();
        break;

      case 'b':
        // Serial1.println("revers b");
        indexPositionDistance--;
        MovementForwardUsingDistanceAutomated();
        indexPositionMovement--;
        indexPositionDistance--;
        TurnRight(90);
        TurnRight(90);
        MovementForwardUsingDistanceAutomated();
        break;

      case 'n':
        indexPositionMovement--;
        indexPositionDistance--;
        MovementForwardUsingDistanceAutomated();

    }
  }
}

void SwitchCaseForSearchingRoomInMovement()
{
  bool turnOffLoop = false;
  do {
    incomingByte = Serial1.read();

    switch (incomingByte)
    {
      case't':
        Serial1.println("Manual: Moving Forward");
        MotorSpeedForward();
        delay(10);
        MotorSpeedStop();
        Serial1.flush();
        break;

      case 'f':
        Serial1.println("Adjust: Turning Left");
        MotorSpeedTurnLeft();
        delay(10);
        MotorSpeedStop();
        turnSensorReset();
        Serial1.flush();
        break;

      case 'y':
        Serial1.println("Adjust: Turning Right");
        MotorSpeedTurnRight();
        delay(10);
        MotorSpeedStop();
        turnSensorReset();
        Serial1.flush();
        break;

      case 'g':
        Serial1.println("Adjust: Moving Backwards");
        MotorSpeedBackward();
        delay(10);
        MotorSpeedStop();
        Serial1.flush();
        break;

      case 'i':
        AddEncoderValues(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
        Serial1.println("Automated: Moving Forward");
        MovementGoingForward();
        Serial1.flush();
        turnOffLoop = true;
        break;

      case 'z':
        AddEncoderValues(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
        AddEncoderValuesIntoArray();
        Serial1.println("Automated: Searching Room On The Left");
        AddMovementValueIntoArray('x');
        TurnLeftScanRoom();
        turnOffLoop = true;
        Serial1.flush();
        break;

      case 'x':
        AddEncoderValues(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
        AddEncoderValuesIntoArray();
        Serial1.println("Automated: Searching Room On The Right");
        AddMovementValueIntoArray('z');
        TurnRightScanRoom();
        turnOffLoop = true;
        Serial1.flush();
        break;

      case 'e':
        MotorSpeedStop();
        Serial1.println("Stopped");
        Serial1.flush();
        break;
    }
  } while (turnOffLoop != true);
}

void TurnLeftScanRoom()
{
  MotorSpeedStop();
  delay(100);
  TurnLeft(90);
  ScanRoom();
  delay(400);
  TurnRight(90);
  //MovementGoingForward();
}
void TurnRightScanRoom()
{
  MotorSpeedStop();
  delay(100);
  TurnRight(90);
  ScanRoom();
  delay(400);
  TurnLeft(90);
  // MovementGoingForward();
}
void ScanRoom()
{
  bool objectSeen = false;
  delay(200);
  objectSeen = ScanRoomProximityTurnLeftGyro(objectSeen, 45);
  delay(200);
  objectSeen = ScanRoomProximityTurnRightGyro(objectSeen, 45);
  delay(200);
  objectSeen = ScanRoomProximityTurnRightGyro(objectSeen, 45);
  delay(200);
  objectSeen = ScanRoomProximityTurnLeftGyro(objectSeen, 45);
  if (!ReturnHomeIsTrue)
  {
    SearchRoomMessagesBeforeReturningHome(objectSeen);
  }
  else
  {
    SearchRoomMeessagesReturningHome(objectSeen);
  }
}

void SearchRoomMessagesBeforeReturningHome(bool objectseen)
{
  if (objectseen) {
    Serial1.print("#Room ");
    Serial1.print(AmountOfRooms);
    Serial1.println(" Searched: Survivor Inside !");
    AmountOfRooms++;
    AddBoolValueToArray(true, true);

  } else {
    //Change it to n which stands for nothing, means we can skip searching a room in the automation
    //- 1 since when it was added position would have been incremented.
    movementArray[indexPositionMovement - 1] = 'n';
    Serial1.print("#Room ");
    Serial1.print(AmountOfRooms);
    Serial1.println("Searched: Room Empty !");
    AddBoolValueToArray(true, false);
  }
}

void SearchRoomMeessagesReturningHome(bool objectseen)
{
    if (objectseen) {
    Serial1.println(" Searched: Survivor Inside Sending Signal! ");
    SurvivorFollowing = true;
    PlayBuzzer();
    FlashLED();  
  } else {
    Serial1.println("Searched: Survivor Has Left The Room");
  }
}

void AddEncoderValues( int leftencodervalue, int rightencodervalue)
{
  totalDistanceValueRightEncoder += leftencodervalue;
  totalDistanceValueLeftEncoder += rightencodervalue;
  Serial1.println(totalDistanceValueRightEncoder);
  Serial1.println(totalDistanceValueLeftEncoder);
}
void ResetEncoderTotalValues()
{
  totalDistanceValueRightEncoder = 0;
  totalDistanceValueLeftEncoder = 0;
}

void AddEncoderValuesIntoArray()
{
  rightDistanceEncoderArray[indexPositionDistance] = totalDistanceValueRightEncoder;
  leftDistanceEncoderArray[indexPositionDistance]  = totalDistanceValueLeftEncoder;
  indexPositionDistance++;
}

void AddMovementValueIntoArray(char movement)
{
  movementArray[indexPositionMovement] = movement;
  indexPositionMovement++;
}

void AddBoolValueToArray(bool hasroom, bool hascivilian)
{
  RoomHasCivilian[indexPositionCorridorRooms] = hascivilian;
  CorridorHasRoom[indexPositionCorridorRooms] = hasroom;
  indexPositionCorridorRooms++;
}

void InitArrays()
{
  indexPositionDistance = 0;
  indexPositionMovement = 0;
  indexPositionCorridorRooms = 0;
  for (int i = 0; i < 20; i++)
  {
    RoomHasCivilian[i] = false;
    CorridorHasRoom[i] = false;
    rightDistanceEncoderArray[i] = 0;
    leftDistanceEncoderArray[i]  = 0;
    movementArray[i] = '#';
  }
}


void MovementForwardUsingDistance()
{
  turnSensorReset();
  int error;
  int correction;
  int currentSpeedLeft = MAIN_SPEED;
  int currentSpeedRight = MAIN_SPEED;
  int tempLeft = 0;
  int tempRight  = 0;

  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  int countsLeft =  encoders.getCountsLeft();
  int countsRight = encoders.getCountsRight();
  motors.setSpeeds(MAIN_SPEED, MAIN_SPEED);
  //Serial1.println(totalDistanceValueLeftEncoder);
  //Serial1.println(totalDistanceValueRightEncoder);
  while (countsLeft + tempLeft < totalDistanceValueLeftEncoder + TJUNCTION && countsRight + tempRight < totalDistanceValueRightEncoder + TJUNCTION)
  {
    delay(2);
    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsLeft - STRAIGHTFACTOR * countsRight;
    correction = Kp * error;
    currentSpeedRight = MAIN_SPEED + correction;
    motors.setSpeeds(currentSpeedLeft, currentSpeedRight);
    lineSensors.read(lineSensorValues);
    printReadingsToSerial();
    if (lineSensorValues[2] > QTR_THRESHOLD || lineSensorValues[0] > QTR_THRESHOLD)
    {
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      MotorSpeedStop();

      if (lineSensorValues[2] > QTR_THRESHOLD)
      {
        motors.setSpeeds(-REVERSE_SPEED, REVERSE_SPEED);
        delay(TURN_DURATION);
        MotorSpeedStop();
        tempLeft += encoders.getCountsAndResetLeft();
        tempRight += encoders.getCountsAndResetRight();
      }
      else
      {
        motors.setSpeeds(REVERSE_SPEED, -REVERSE_SPEED);
        delay(TURN_DURATION);
        MotorSpeedStop();
        tempLeft += encoders.getCountsAndResetLeft();
        tempRight += encoders.getCountsAndResetRight();
      }

      countsLeft = encoders.getCountsLeft();
      countsRight = encoders.getCountsRight();
    }
  }
  Serial1.println(countsLeft + tempLeft);
  Serial1.println(countsRight + tempRight);
}


void MovementForwardUsingDistance(int distance)
{
  turnSensorReset();
  int error;
  int correction;
  int currentSpeedLeft = MAIN_SPEED;
  int currentSpeedRight = MAIN_SPEED;
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  int countsLeft =  encoders.getCountsLeft();
  int countsRight = encoders.getCountsRight();

  motors.setSpeeds(MAIN_SPEED, MAIN_SPEED);
  while (countsLeft < distance && countsRight < distance)
  {
    delay(2);
    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsLeft - STRAIGHTFACTOR * countsRight;
    correction = Kp * error;
    currentSpeedRight = MAIN_SPEED + correction;
    motors.setSpeeds(currentSpeedLeft, currentSpeedRight);
    //lineSensors.read(lineSensorValues);
    //printReadingsToSerial();
    //    if (lineSensorValues[2] > QTR_THRESHOLD || lineSensorValues[0] > QTR_THRESHOLD)
    //    {
    //      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    //      delay(REVERSE_DURATION);
    //      MotorSpeedStop();
    //
    //      if (lineSensorValues[2] > QTR_THRESHOLD)
    //      {
    //        motors.setSpeeds(-REVERSE_SPEED, REVERSE_SPEED);
    //        delay(TURN_DURATION);
    //        MotorSpeedStop();
    //      }
    //      else
    //      {
    //        motors.setSpeeds(REVERSE_SPEED, -REVERSE_SPEED);
    //        delay(TURN_DURATION);
    //        MotorSpeedStop();
    //      }
    //    }
    //    printReadingsToSerial();
  }
}


void MovementForwardUsingDistanceAutomated()
{
  turnSensorReset();
  int error = 0;
  int correction = 0;
  int currentSpeedLeft = MAIN_SPEED;
  int currentSpeedRight = MAIN_SPEED;
  int tempLeft = 0;
  int tempRight  = 0;

  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  int countsLeft =  encoders.getCountsLeft();
  int countsRight = encoders.getCountsRight();
  motors.setSpeeds(MAIN_SPEED, MAIN_SPEED);

  while (countsLeft + tempLeft < leftDistanceEncoderArray[indexPositionDistance] && countsRight + tempRight < rightDistanceEncoderArray[indexPositionDistance])
  {
    delay(2);
    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsLeft - STRAIGHTFACTOR * countsRight;
    correction = Kp * error;
    currentSpeedRight = MAIN_SPEED + correction;
    motors.setSpeeds(currentSpeedLeft, currentSpeedRight);
    lineSensors.read(lineSensorValues);
    if (lineSensorValues[2] > QTR_THRESHOLD || lineSensorValues[0] > QTR_THRESHOLD)
    {
      motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
      delay(REVERSE_DURATION);
      MotorSpeedStop();

      if (lineSensorValues[2] > QTR_THRESHOLD)
      {
        motors.setSpeeds(-REVERSE_SPEED, REVERSE_SPEED);
        delay(TURN_DURATION);
        MotorSpeedStop();
        tempLeft += encoders.getCountsAndResetLeft();
        tempRight += encoders.getCountsAndResetRight();
      }
      else
      {
        motors.setSpeeds(REVERSE_SPEED, -REVERSE_SPEED);
        delay(TURN_DURATION);
        MotorSpeedStop();
        tempLeft += encoders.getCountsAndResetLeft();
        tempRight += encoders.getCountsAndResetRight();
      }

      countsLeft = encoders.getCountsLeft();
      countsRight = encoders.getCountsRight();
    }
  }
  MotorSpeedStop();
}
