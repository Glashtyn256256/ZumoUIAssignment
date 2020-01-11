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
#define STRAIGHTFACTOR 1  // Adjust this to correct for minor curve.  Should be in the 0.9 to 1.1 range
#define DEGREES 90

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

bool AutomationIsTrue = false;

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  Serial.begin(9600);
  indexPositionDistance = 0;
  indexPositionMovement = 0;
  indexPositionCorridorRooms = 0;
  InitArrays();
  lineSensors.initThreeSensors();
  proxSensors.initFrontSensor();
  //  buttonA.waitForButton();
  turnSensorSetup();
  lineSensorCalibrateSetup();
  Serial1.print(readBatteryMillivolts());

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
  Serial1.println(encoders.getCountsAndResetLeft());
  Serial1.println(encoders.getCountsAndResetRight());
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
      MovementGoingForward();
      break;

    case 'a':
      TurnLeft(90);
      AddMovementValueIntoArray('d');
      break;

    case 'd':
      TurnRight(90);
      AddMovementValueIntoArray('a');
      break;

    case 's':
      MotorSpeedBackward();
      turnSensorUpdate();
      //delay(2);
      break;

    case 'z':
      TurnLeftUsingEncoders();
      break;

    case 'x':
      TurnRightUsingEncoders();
      break;

    case 'e':
      TurnLeft(90);
      TurnLeft(90);
      ResetEncoderTotalValues();
      if (CorridorHasRoom[indexPositionCorridorRooms - 1])
      {
        totalDistanceValueRightEncoder += rightDistanceEncoderArray[indexPositionMovement - 1];
        totalDistanceValueRightEncoder += rightDistanceEncoderArray[indexPositionMovement - 2];
        totalDistanceValueLeftEncoder += leftDistanceEncoderArray[indexPositionMovement - 1];
        totalDistanceValueLeftEncoder += leftDistanceEncoderArray[indexPositionMovement - 2];
      }
      else
      {
        totalDistanceValueRightEncoder += rightDistanceEncoderArray[indexPositionMovement - 1];
        totalDistanceValueLeftEncoder += leftDistanceEncoderArray[indexPositionMovement - 1];
      }
      AddEncoderValuesIntoArray();
      AddMovementValueIntoArray('b');
      MovementForwardUsingDistance();
      MotorSpeedStop();
      break;
    case 'b':
      MotorSpeedStop();
      Serial1.print("Stopped");
      break;

      case 'h':
         AutomationIsTrue = true;
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
  //turnSensorReset();
  motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
  int angle = 0;
  do {
    delay(1);
    turnSensorUpdate();
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
  } while (angle > -degrees);
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  motors.setSpeeds(0, 0);
  turnSensorReset();
}

void TurnLeft(int degrees) {
  //turnSensorReset();
  motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
  int angle = 0;
  do {
    delay(1);
    turnSensorUpdate();
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
  } while (angle < degrees);
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  motors.setSpeeds(0, 0);
  turnSensorReset();
}

void ScanRoom()
{
  bool objectSeen = false;
  delay(200);
  objectSeen = ScanRoomProximityTurnLeftGyro(objectSeen, DEGREES);
  delay(200);
  objectSeen = ScanRoomProximityTurnRightGyro(objectSeen, DEGREES);
  delay(200);
  objectSeen = ScanRoomProximityTurnRightGyro(objectSeen, DEGREES);
  delay(200);
  objectSeen = ScanRoomProximityTurnRightGyro(objectSeen, DEGREES);
  if (!AutomationIsTrue) {
    if (objectSeen) {
      Serial1.print("TRUE");
      AddBoolValueToArray(true, true);

    } else {
      //Change it to n which stands for nothing, means we can skip searching a room in the automation
      //- 1 since when it was added position would have been incremented.
      movementArray[indexPositionMovement - 1] = 'n';
      Serial1.print("FALSE");
    }
  }
}

bool ScanRoomProximityTurnLeftGyro(bool objectseen, int degrees)
{
  MotorSpeedTurnLeft();
  turnSensorReset();
  int angle = 0;
  do {
    delay(1);
    turnSensorUpdate();
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    if (objectseen == false)
    {
      proxSensors.read();
      leftValue = proxSensors.countsFrontWithLeftLeds();
      rightValue = proxSensors.countsFrontWithRightLeds();
      // printReadingsToSerials();
      objectseen = leftValue >= sensorProximityThreshold || rightValue >= sensorProximityThreshold;
    }
  } while (angle < degrees);
  MotorSpeedStop();
  turnSensorReset();
  return objectseen;
}

bool ScanRoomProximityTurnRightGyro(bool objectseen, int degrees)
{
  MotorSpeedTurnRight();
  turnSensorReset();
  int angle = 0;
  do {
    delay(1);
    turnSensorUpdate();
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    if (objectseen == false)
    {
      proxSensors.read();
      leftValue = proxSensors.countsFrontWithLeftLeds();
      rightValue = proxSensors.countsFrontWithRightLeds();
      // printReadingsToSerials();
      objectseen = leftValue >= sensorProximityThreshold || rightValue >= sensorProximityThreshold;
    }
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
    delay(2);
    incomingByte = Serial1.read();
    if (incomingByte == 'e' || incomingByte == 'z' || incomingByte == 'x')
    {
      MotorSpeedStop();
      AddEncoderValues(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
      AddEncoderValuesIntoArray();
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
    printReadingsToSerial();
    if (lineSensorValues[2] > QTR_THRESHOLD || lineSensorValues[0] > QTR_THRESHOLD)
    {
      printReadingsToSerial();
      delay(50);
      lineSensors.read(lineSensorValues);
      printReadingsToSerial();
      if ((lineSensorValues[0] > QTR_THRESHOLD
           && lineSensorValues[1] > QTR_THRESHOLD)
          || (lineSensorValues[2] > QTR_THRESHOLD
              && lineSensorValues[1] > QTR_THRESHOLD))
      {
        motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
        delay(150);
        MotorSpeedStop();
        AddEncoderValues(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
        AddEncoderValuesIntoArray();
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
    switch (incomingByte)
    {
      case 'a':
        TurnLeft(90);
        indexPositionMovement--;
        indexPositionDistance--;
        MovementForwardUsingDistanceAutomated();
        break;

      case 'd':
        TurnRight(90);
        indexPositionMovement--;
        indexPositionDistance--;
        MovementForwardUsingDistanceAutomated();
        break;

      case 'z':
        TurnLeftScanRoom();
        indexPositionMovement--;
        indexPositionDistance--;
        MovementForwardUsingDistanceAutomated();
        break;

      case 'x':
        TurnRightScanRoom();
        indexPositionMovement--;
        indexPositionDistance--;
        MovementForwardUsingDistanceAutomated();
        break;
      case 'b':
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
  switch (incomingByte)
  {
    case 'z':
      AddMovementValueIntoArray('x');
      TurnRightScanRoom();
      break;

    case 'x':
      AddMovementValueIntoArray('z');
      TurnRightScanRoom();
      break;

    case 'e':
      MotorSpeedStop();
      Serial1.print("Stopped");
      break;
  }
}

void TurnLeftScanRoom()
{
  MotorSpeedStop();
  delay(100);
  TurnLeft(90);
  MovementForwardUsingDistance(50);
  ScanRoom();
  MovementForwardUsingDistance(60);
  delay(100);
  TurnLeft(90);
  delay(100);
  //MovementGoingForward();
}
void TurnRightScanRoom()
{
  MotorSpeedStop();
  delay(100);
  TurnRight(90);
  MovementForwardUsingDistance(50);
  ScanRoom();
  delay(100);
  MovementForwardUsingDistance(60);
  delay(100);
  TurnRight(90);
  delay(100);
  //MovementGoingForward();
}
void AddEncoderValues( int leftencodervalue, int rightencodervalue)
{
  totalDistanceValueRightEncoder = leftencodervalue;
  totalDistanceValueLeftEncoder = rightencodervalue;
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
  while (countsLeft + tempLeft < totalDistanceValueLeftEncoder && countsRight + tempRight < totalDistanceValueRightEncoder)
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
    }
    printReadingsToSerial();
  }
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
  Serial1.println("hit");
  while (countsLeft < distance && countsRight < distance)
  {
    Serial1.println("inside");
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
      }
      else
      {
        motors.setSpeeds(REVERSE_SPEED, -REVERSE_SPEED);
        delay(TURN_DURATION);
        MotorSpeedStop();
      }
    }
    printReadingsToSerial();
  }
}


void MovementForwardUsingDistanceAutomated()
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
    }
    printReadingsToSerial();
  }
}
