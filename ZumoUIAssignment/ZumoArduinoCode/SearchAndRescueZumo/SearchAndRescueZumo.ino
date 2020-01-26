#include <Zumo32U4.h>
#include "TurnSensor.h"
#include "BuzzerAndLED.h"
#include "SerialMessages.h"
#include "MotorSpeedAndDuration.h"

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


/*The number of line sensors and the data we store in the array*/
#define NUM_SENSORS 3
uint16_t lineSensorValues[NUM_SENSORS];
#define QTR_THRESHOLD     500  // microseconds
#define LINE_SENSOR_DELAY 50

//Used for the proximity sensors to scan the room.
const uint16_t sensorProximityThreshold = 5;
uint16_t leftValue;
uint16_t rightValue;



#define Kp 1
#define STRAIGHTFACTOR 1.015  // Adjust this to correct for minor curve.  Should be in the 0.9 to 1.1 range
#define DEGREES 90

//TJUNCTION was originally used for how far we wanted to get past the T junction. Not needed now
#define TJUNCTION 0

//Store the left and right encoder values for each distance.
int rightDistanceEncoderArray[20];
int leftDistanceEncoderArray[20];
int totalDistanceValueRightEncoder;
int totalDistanceValueLeftEncoder;
int indexPositionDistance;

//Store any rotations, room searches or T junction movements into this array.
//Both distance and movement array allow us to return home automatically.
char movementArray[20];
int indexPositionMovement;

//Amount of rooms we've currently been in
int AmountOfRooms;
//Boolean to let certain functions know we are returning home
bool ReturnHomeIsTrue;

//All used for movement and roation.
int error;
int correction;
int currentSpeedLeft;
int currentSpeedRight;
int countsLeft;
int countsRight;
int tempLeft;
int tempRight;

/*This is called on all movement functions such as rotating and moving forward
  this resets the encoders and sets the wheels to the correct speed.*/
void SetEncodersAndSpeed(int leftspeed, int rightspeed)
{
  error = 0;
  correction = 0;
  currentSpeedLeft = leftspeed;
  currentSpeedRight = rightspeed;

  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  countsLeft =  encoders.getCountsLeft();
  countsRight = encoders.getCountsRight();
  tempLeft = 0;
  tempRight = 0;
  SetSpeedValues(leftspeed, rightspeed);
}

void setup() {
  //Our Serials that we use to send or read data. Serial1 is over XBEE and Serial is over USB cable
  Serial1.begin(9600);
  Serial.begin(9600);

  //Resets our arrays and index positions.
  InitArrays();

  //Sets up the lines sensors and proximity sensors.
  lineSensors.initThreeSensors();
  proxSensors.initFrontSensor();

  //Calibrates the Gyro
  turnSensorSetup();

  //Calibrates the line sensors.
  lineSensorCalibrateSetup();

  //Displays how much voltage we currently have in our batteries in the zumo.
  Serial1.print("Battery Power: ");
  Serial1.println(readBatteryMillivolts());

}

bool aboveLine(uint8_t sensorIndex)
{
  return lineSensorValues[sensorIndex] > QTR_THRESHOLD;
}

char incomingByte;
void loop() {

  incomingByte = Serial1.read();

  /*Main switch, I mean it's pretty obvious what each case does due to the naming of the variables
    but in short this switch allows us to adjust our zumo position, move manually, automated movement,
    180 turn and return home automatically. This switch is called at the start and when we hit the end
    of a corridor*/
  switch (incomingByte)
  {
    case'w':
      ManualMessage();
      MoveForwardMessage();
      ManualForwardSpeed();
      Serial1.flush();
      break;

    case 'a':
      ManualMessage();
      TurnLeftMessage();
      ManualTurnLeftSpeed();
      Serial1.flush();
      break;

    case 'd':
      ManualMessage();
      TurnRightMessage();
      ManualTurnRightSpeed();
      Serial1.flush();
      break;

    case 's':
      ManualMessage();
      MoveBackwardsMessage();
      ManualReverseSpeed();
      Serial1.flush();
      break;

    case't':
      AdjustMessage();
      MoveForwardMessage();
      SetSpeedValuesDurationAndStop(ADJUST_FORWARD_SPEED, ADJUST_FORWARD_SPEED, ADJUST_DURATION);
      Serial1.flush();
      break;

    case 'f':
      AdjustMessage();
      TurnLeftMessage();
      SetSpeedValuesDurationAndStop(-ADJUST_TURN_SPEED, ADJUST_TURN_SPEED, ADJUST_DURATION);
      turnSensorReset();
      Serial1.flush();
      break;

    case 'y':
      AdjustMessage();
      TurnRightMessage();
      SetSpeedValuesDurationAndStop(ADJUST_TURN_SPEED, -ADJUST_TURN_SPEED, ADJUST_DURATION);
      turnSensorReset();
      Serial1.flush();
      break;

    case 'g':
      AdjustMessage();
      MoveBackwardsMessage();
      SetSpeedValuesDurationAndStop(-ADJUST_REVERSE_SPEED, -ADJUST_REVERSE_SPEED, ADJUST_DURATION);
      Serial1.flush();
      break;

    case 'e':
      SpeedStop();
      turnSensorReset();
      MovementStoppedMessage();
      Serial1.flush();
      break;

    case 'q':
      ManualMessage();
      TurnLeftDegreesMessage();
      TurnLeft(90);
      AddMovementValueIntoArray('d');
      ManualMessage();
      TurningCompletedMessage();
      PressToContinueMessage();
      Serial1.flush();
      break;

    case 'r':
      ManualMessage();
      TurnRightDegreesMessage();
      TurnRight(90);
      AddMovementValueIntoArray('a');
      ManualMessage();
      TurningCompletedMessage();
      PressToContinueMessage();
      Serial1.flush();
      break;

    case'i': case'c':
      AutomatedMessage();
      MoveForwardMessage();
      //Reset the encoder values since want to start fresh when we call the movementgoingforward in this switch
      ResetEncoderTotalValues();
      MovementGoingForward();
      Serial1.flush();
      break;

    case 'j':
      AutomatedMessage();
      TurnLeftDegreesMessage();
      TurnLeft(90);
      AutomatedMessage();
      TurningCompletedMessage();
      //Add the opposite turn into the array, since on the way back it will be opposite of what you inputted.
      AddMovementValueIntoArray('d');
      delay(100);
      AutomatedMessage();
      MoveForwardMessage();
      ResetEncoderTotalValues();
      MovementGoingForward();
      Serial1.flush();
      break;

    case 'l':
      AutomatedMessage();
      TurnRightDegreesMessage();
      TurnRight(90);
      AutomatedMessage();
      TurningCompletedMessage();
      //Add the opposite turn into the array, since on the way back it will be opposite of what you inputted.
      AddMovementValueIntoArray('a');
      delay(100);
      AutomatedMessage();
      MoveForwardMessage();
      ResetEncoderTotalValues();
      MovementGoingForward();
      Serial1.flush();
      break;

    case 'b':
      AutomatedMessage();
      TurningHalfDegreesMessage();
      delay(200);
      TurnLeft(90);
      delay(200);
      TurnLeft(90);
      delay(200);
      ResetEncoderTotalValues();

      /*This checks to see if the corridor we are in has a room or doesn't have a room.
        if it has a room then we need to add the two distances together, if it doesnt have
        a room then we only want the whole distance !!!This will not work if a corridor has
        two rooms and we press B, can only work with one room in a corridor!!!*/
      if (movementArray[indexPositionMovement - 1] == 'x' ||
          movementArray[indexPositionMovement - 1] == 'z' ||
          movementArray[indexPositionMovement - 1] == 'n')
      {
        AddEncoderValues(leftDistanceEncoderArray[indexPositionDistance - 1] , rightDistanceEncoderArray[indexPositionDistance - 1]);
        AddEncoderValues(leftDistanceEncoderArray[indexPositionDistance - 2] , rightDistanceEncoderArray[indexPositionDistance - 2]);
      }
      else
      {
        AddEncoderValues(leftDistanceEncoderArray[indexPositionDistance - 1] , rightDistanceEncoderArray[indexPositionDistance - 1]);
        //This was used for testing but cant remove because if you do it throws a error on LEDlightsOn wtf?
      }
      AddEncoderValuesIntoArray();
      AddMovementValueIntoArray('b');

      //Allows us to adjust the zumo when it has done a 180 degree turn. Can only move forward and adjust.
      SwitchCaseForReturningToJunction();

      AutomatedMessage();
      ArrivedTJunctionMessage();
      //SpeedStop();

      /*This checks to see if the room we have searched is valid, if it is a z or x it means a survior was inside
        and we want to come back to search the room. If it wasn't and it was a n or another value then we want to
        cut out this corridor for when we return home this optimizes the path. !!!This will not work if a corridor has
        two rooms and there is a surivor at the end, since it only checks the first room.!!!*/
      if (!(movementArray[indexPositionMovement - 2] == 'z' || movementArray[indexPositionMovement - 2] == 'x'))
      {
        if (movementArray[indexPositionMovement - 2] == 'n')
        {
          if (movementArray[indexPositionMovement - 3] == 'a')
          {
            movementArray[indexPositionMovement - 3] = 'd';
          }
          else
          {
            movementArray[indexPositionMovement - 3] = 'a';
          }
          indexPositionDistance -= 3;
          indexPositionMovement -= 2;
        }
        else
        {
          if (movementArray[indexPositionMovement - 2] == 'a')
          {
            movementArray[indexPositionMovement - 2] = 'd';
          }
          else
          {
            movementArray[indexPositionMovement - 2] = 'a';
          }
          indexPositionDistance -= 2;
          indexPositionMovement -= 1;
        }
      }


      //Set to zero
      ResetEncoderTotalValues();
      //Add certain distance to the encoder values so we can bypass the t junction.
      //Been told we can stop at junction so it's not needed.
      //AddEncoderValues(TJUNCTION, TJUNCTION);
      Serial1.flush();
      MovementGoingForward();
      Serial1.flush();
      break;

    case 'h':
      /*This is set to true so certain functions such as scan room can peform differently.*/
      ReturnHomeIsTrue = true;

      /*Remember we increment everytime we add onto the array so we need to decrement
        to get the last value added in the array.*/
      indexPositionMovement--;
      indexPositionDistance--;
      TurnLeft(90);
      TurnLeft(90);
      SwitchCaseForAutomaticBaseReturn();
      break;
  }
}

/*This switch is called only in our moveforward function, when you press e the zumo is stopped
  this function is called and this switch allows us either to adjust the position of the zumo,
  scan a room left or right or carry on moving forward.*/
void SwitchCaseForSearchingRoomInMovement()
{
  bool turnOffLoop = false;
  do {
    incomingByte = Serial1.read();

    switch (incomingByte)
    {
      //Allows us to adjust the position of the Zumo handy if the angle is a bit off.
      case't':
        AdjustMessage();
        MoveForwardMessage();
        SetSpeedValuesDurationAndStop(ADJUST_FORWARD_SPEED, ADJUST_FORWARD_SPEED, ADJUST_DURATION);
        Serial1.flush();
        break;

      case 'f':
        AdjustMessage();
        TurnLeftMessage();
        SetSpeedValuesDurationAndStop(-ADJUST_TURN_SPEED, ADJUST_TURN_SPEED, ADJUST_DURATION);
        turnSensorReset();
        Serial1.flush();
        break;

      case 'y':
        AdjustMessage();
        TurnRightMessage();
        SetSpeedValuesDurationAndStop(ADJUST_TURN_SPEED, -ADJUST_TURN_SPEED, ADJUST_DURATION);
        turnSensorReset();
        Serial1.flush();
        break;

      case 'g':
        AdjustMessage();
        MoveBackwardsMessage();
        SetSpeedValuesDurationAndStop(-ADJUST_REVERSE_SPEED, -ADJUST_REVERSE_SPEED, ADJUST_DURATION);
        Serial1.flush();
        break;

      case 'i':
        /*We need to add the encoder values, store them and reset the encoders.
          If we move forward again, then we can add it back onto the stored values
          giving us the total distance when we hit a wall*/
        AddEncoderValues(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
        AutomatedMessage();
        MoveForwardMessage();
        MovementGoingForward();
        Serial1.flush();
        turnOffLoop = true;
        break;

      case 'z':
        /*We want to know the distance of when we pressed Z on the encoders. This allows us to know
          When we need to scan the room when we return home. So every corridor which has a room or more
          should have two distances each room. e.g distance, room, distance, room, distance, hit end of corridor*/
        AddEncoderValues(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
        AddEncoderValuesIntoArray();
        AddMovementValueIntoArray('x');

        AutomatedMessage();
        SearchRoomMessage();

        TurnLeftScanRoom();
        turnOffLoop = true;
        Serial1.flush();
        break;

      case 'x':
        /*We want to know the distance of when we pressed X on the encoders. This allows us to know
          When we need to scan the room when we return home. So every corridor which has a room or more
          should have two distances each room. e.g distance, room, distance, room, distance, hit end of corridor*/
        AddEncoderValues(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
        AddEncoderValuesIntoArray();
        AddMovementValueIntoArray('z');

        AutomatedMessage();
        SearchRoomMessage();
        TurnRightScanRoom();
        turnOffLoop = true;
        Serial1.flush();
        break;

      case 'e':
        SpeedStop();
        MovementStoppedMessage();
        Serial1.flush();
        break;
    }
  } while (turnOffLoop != true);
}

/*This switch is called only when we press b, the reason for this usually the turning 180 isn't reliable enough and
  it will be a little off. This allows us to adjust the position of the zumo and then move forward when we are happy
  with the position. Asked Mark and he said it's fine so noice.*/
void SwitchCaseForReturningToJunction()
{
  bool turnOffLoop = false;
  do {
    incomingByte = Serial1.read();

    switch (incomingByte)
    {
      case't':
        AdjustMessage();
        MoveForwardMessage();
        SetSpeedValuesDurationAndStop(ADJUST_FORWARD_SPEED, ADJUST_FORWARD_SPEED, ADJUST_DURATION);
        Serial1.flush();
        break;

      case 'f':
        AdjustMessage();
        TurnLeftMessage();
        SetSpeedValuesDurationAndStop(-ADJUST_TURN_SPEED, ADJUST_TURN_SPEED, ADJUST_DURATION);
        turnSensorReset();
        Serial1.flush();
        break;

      case 'y':
        AdjustMessage();
        TurnRightMessage();
        SetSpeedValuesDurationAndStop(ADJUST_TURN_SPEED, -ADJUST_TURN_SPEED, ADJUST_DURATION);
        turnSensorReset();
        Serial1.flush();
        break;

      case 'g':
        AdjustMessage();
        MoveBackwardsMessage();
        SetSpeedValuesDurationAndStop(-ADJUST_REVERSE_SPEED, -ADJUST_REVERSE_SPEED, ADJUST_DURATION);
        Serial1.flush();
        break;

      case 'i':
        AutomatedMessage();
        MoveForwardMessage();
        MovementForwardUsingDistance();
        Serial1.flush();
        turnOffLoop = true;
        break;
    }
  } while (turnOffLoop != true);
}

/*This is very important this switch is called when we press H and want to return home
  We move forward first using the encoder value we receive from the distance array to start
  us off. It will then enter a while and then check the movement array to see what movement
  needs to be done. Example it's 'a' we rotate left, decrement both index positions for the
  distance and mvoement array and then Moveforward. This cycle will keep happening till we
  return home.

  !!!Warning: If our arrays go forward, rotate and rotate again. this will break the
  returning home. It needs to go distance, movement, distance. Not distance, movement, movement, distance
  otherwise this won't work.*/
void SwitchCaseForAutomaticBaseReturn()
{
  //Do the first movement, this starts our zumo off with first distance.
  //REMEMBER! We want to be going backwards in our arrays
  MovementForwardUsingDistanceAutomated();
  while (true)
  {
    char nextMovement = movementArray[indexPositionMovement];
    switch (nextMovement)
    {
      case 'a':
        ReturningHomeMessage();
        TurnLeftMessage();
        TurnLeft(90);
        indexPositionMovement--;
        indexPositionDistance--;
        ReturningHomeMessage();
        MoveForwardMessage();
        MovementForwardUsingDistanceAutomated();
        break;

      case 'd':
        ReturningHomeMessage();
        TurnRightMessage();
        TurnRight(90);
        indexPositionMovement--;
        indexPositionDistance--;
        ReturningHomeMessage();
        MoveForwardMessage();
        MovementForwardUsingDistanceAutomated();
        break;

      case 'z':
        ReturningHomeMessage();
        SearchRoomMessage();
        TurnLeftScanRoom();
        indexPositionMovement--;
        indexPositionDistance--;
        ReturningHomeMessage();
        MoveForwardMessage();
        MovementForwardUsingDistanceAutomated();
        break;

      case 'x':
        ReturningHomeMessage();
        SearchRoomMessage();
        TurnRightScanRoom();
        indexPositionMovement--;
        indexPositionDistance--;
        ReturningHomeMessage();
        MoveForwardMessage();
        MovementForwardUsingDistanceAutomated();
        break;

      case 'b':
        ReturningHomeMessage();
        MoveForwardMessage();
        indexPositionDistance--;
        MovementForwardUsingDistanceAutomated();

        indexPositionMovement--;
        indexPositionDistance--;
        ReturningHomeMessage();
        TurningHalfDegreesMessage();
        TurnRight(90);
        TurnRight(90);

        ReturningHomeMessage();
        MoveForwardMessage();
        MovementForwardUsingDistanceAutomated();
        break;

      case 'n':
        indexPositionMovement--;
        indexPositionDistance--;
        ReturningHomeMessage();
        MoveForwardMessage();
        MovementForwardUsingDistanceAutomated();

    }

    if (indexPositionDistance == 0)
    {
      TurnOffLED();
      ReturningHomeMessage();
      ReturnedBackToHomeBaseMessage();
      InitArrays();
      break;
    }
  }
}


/*Rotates right using the gyro to detect what angle we are at and the encoders
  to keep the motors speed even (since motors have different power)*/
void TurnRight(int degrees)
{
  turnSensorReset();
  SetEncodersAndSpeed(AUTO_TURN_SPEED, -AUTO_TURN_SPEED);
  int angle = 0;
  do {
    delay(1);
    countsLeft = encoders.getCountsLeft();
    countsRight = -encoders.getCountsRight();
    error = countsRight - countsLeft;
    correction = Kp * error;
    currentSpeedLeft = AUTO_TURN_SPEED + correction;
    SetSpeedValues(currentSpeedLeft, currentSpeedRight);
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    turnSensorUpdate();

    /*Even when correcting the motors speed for some reason it still cant turn correctly
    when turning right. Even when I hard coded the speed, I decided to add extra degrees on and 
    it seems to perform a full right turn or 45* turn. Hence any value passed through will have negative 3
    onto it.
    */
  } while (angle > -degrees - 2);
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  SpeedStop();
  turnSensorReset();
}

/*Rotates left using the gyro to detect what angle we are at and the encoders
  to keep the motors speed even (since motors have different power)*/
void TurnLeft(int degrees)
{
  turnSensorReset();
  SetEncodersAndSpeed(-AUTO_TURN_SPEED, AUTO_TURN_SPEED);
  int angle = 0;
  do {
    delay(1);
    countsLeft = -encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsRight - countsLeft;
    correction = Kp * error;
    currentSpeedLeft = AUTO_TURN_SPEED + correction;
    SetSpeedValues(-currentSpeedLeft, currentSpeedRight);
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    turnSensorUpdate();

    /*TurnLeft seems to work most the time I've added 1 more degree on just in-case
    since I would rather overcompensate than undercompensate*/
  } while (angle < degrees + 2);
  encoders.getCountsAndResetLeft();
  encoders.getCountsAndResetRight();
  SpeedStop();
  turnSensorReset();
}


/*This is the same as turn left and right but scans to see if a object is in the room.
  if it is then we return the bool, we make sure to pass in objectseen so we dont accidentally
  overwrite the value if it's true. So if it's been scanned, it wont use the proximity sensors
  anymore.*/
bool ScanRoomProximityTurnLeftGyro(bool objectseen, int degrees)
{
  turnSensorReset();
  SetEncodersAndSpeed(-AUTO_TURN_SPEED, AUTO_TURN_SPEED);
  int angle = 0;
  do {
    //delay(1);
    countsLeft = -encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsRight - countsLeft;
    correction = Kp * error;
    currentSpeedLeft = AUTO_TURN_SPEED + correction;
    SetSpeedValues(-currentSpeedLeft, currentSpeedRight);
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    if (objectseen == false)
    {
      proxSensors.read();
      leftValue = proxSensors.countsFrontWithLeftLeds();
      rightValue = proxSensors.countsFrontWithRightLeds();
      objectseen = leftValue >= sensorProximityThreshold || rightValue >= sensorProximityThreshold;
    }
    turnSensorUpdate();
  } while (angle < degrees );
  SpeedStop();
  turnSensorReset();
  return objectseen;
}

/*This is the same as turn left and right but scans to see if a object is in the room.
  if it is then we return the bool, we make sure to pass in objectseen so we dont accidentally
  overwrite the value if it's true. So if it's been scanned, it wont use the proximity sensors
  anymore.*/
bool ScanRoomProximityTurnRightGyro(bool objectseen, int degrees)
{
  turnSensorReset();
  SetEncodersAndSpeed(AUTO_TURN_SPEED, -AUTO_TURN_SPEED);
  int angle = 0;
  do {
    delay(1);
    countsLeft = encoders.getCountsLeft();
    countsRight = -encoders.getCountsRight();
    error = countsRight - countsLeft;
    correction = Kp * error;
    currentSpeedLeft = AUTO_TURN_SPEED + correction;
    SetSpeedValues(currentSpeedLeft, currentSpeedRight);
    angle = (((int32_t)turnAngle >> 16) * 360) >> 16;
    if (objectseen == false)
    {
      proxSensors.read();
      leftValue = proxSensors.countsFrontWithLeftLeds();
      rightValue = proxSensors.countsFrontWithRightLeds();
      objectseen = leftValue >= sensorProximityThreshold || rightValue >= sensorProximityThreshold;
    }
    turnSensorUpdate();
  } while (angle > -degrees - 2);
  SpeedStop();
  turnSensorReset();
  return objectseen;
}

void TurnLeftScanRoom()
{
  SpeedStop();
  delay(120);
  TurnLeft(90);
  delay(120);
  MovementForwardSettingDistanceAndSpeed(100,100, 400);
  ScanRoom();
  delay(120);
  MovementReverseSettingDistanceAndSpeed(-100,-100,-400);
  delay(400);
  TurnRight(90);
  
  //MovementGoingForward();
}
void TurnRightScanRoom()
{
  SpeedStop();
  delay(120);
  TurnRight(90);
  delay(120);
  MovementForwardSettingDistanceAndSpeed(100,100,400);
  delay(120);
  ScanRoom();
  delay(120);
  MovementReverseSettingDistanceAndSpeed(-100,-100,-401);
  delay(400);
  TurnLeft(90);
  // MovementGoingForward();
}

/*We call this to scan a room, if a object is found form the scan it will return true. False if nothing was found*/
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

/*Message for if a survivor is in a room or isn't*/
void SearchRoomMessagesBeforeReturningHome(bool objectseen)
{
  if (objectseen) {
    SurvivorMessage(AmountOfRooms);
    AmountOfRooms++;
  } else {
    //Change it to n which stands for nothing, means we can skip searching a room when we return home
    //-1 since when it was added position would have been incremented.
    movementArray[indexPositionMovement - 1] = 'n';
    RoomEmptyMessage(AmountOfRooms);
    AmountOfRooms++;
  }
}

/*Checks to see if a survior is still in the room, if it is it will play a buzzer and flash LED lights
  the lights will stay on and are turned off when we return home. It also lets us know if a survivor has
  left the room*/
void SearchRoomMeessagesReturningHome(bool objectseen)
{
  if (objectseen) {
    SendSignalMessage();
    PlayBuzzer();
    FlashLED();
  } else {
    SurvivorLeftMessage();
  }
}

void AddEncodersToTempLeftAndTempRight(int tleft, int tright) {
  tempLeft = tleft;
  tempRight = tright;
}

void AddEncoderValues( int leftencodervalue, int rightencodervalue)
{
  totalDistanceValueRightEncoder += leftencodervalue;
  totalDistanceValueLeftEncoder += rightencodervalue;
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

void InitArrays()
{
  indexPositionDistance = 0;
  indexPositionMovement = 0;
  //indexPositionCorridorRooms = 0;
  AmountOfRooms = 1;
  ReturnHomeIsTrue = false;

  for (int i = 0; i < 20; i++)
  {
    rightDistanceEncoderArray[i] = 0;
    leftDistanceEncoderArray[i]  = 0;
    movementArray[i] = '#';
  }
}

/*This allows us to move forward using the encoders to keep us in a straight line and the linesensors
  to detect if we have hit a wall. If our left or right sensor has hit a wall then we delay for 60 ms and
  read the values again to see if the middle sensor is over the line. If it is then we chuck the distance
  value into the array. If it hasn't it means we've hit a wall at a angle. We then reverse and then turn for
  a certain amount of item which usually corrects the position of the zumo. We also can Press 'e' to stop this
  takes us to a swtich that allows us to adjust the postition, scan a room or move forward again.*/
void MovementGoingForward()
{
  turnSensorReset();
  SetEncodersAndSpeed(AUTO_FORWARD_SPEED, AUTO_FORWARD_SPEED);

  while (true)
  {
    delay(2);
    incomingByte = Serial1.read();
    if (incomingByte == 'e')
    {
      SpeedStop();
      SwitchCaseForSearchingRoomInMovement();
      break;
    }
    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsLeft - STRAIGHTFACTOR * countsRight;
    correction = Kp * error;
    currentSpeedRight = AUTO_FORWARD_SPEED + correction;
    SetSpeedValues(currentSpeedLeft, currentSpeedRight);

    lineSensors.read(lineSensorValues);
    if (lineSensorValues[2] > QTR_THRESHOLD || lineSensorValues[0] > QTR_THRESHOLD)
    {
      delay(LINE_SENSOR_DELAY);
      lineSensors.read(lineSensorValues);

      if ((lineSensorValues[0] > QTR_THRESHOLD
           && lineSensorValues[1] > QTR_THRESHOLD)
          || (lineSensorValues[2] > QTR_THRESHOLD
              && lineSensorValues[1] > QTR_THRESHOLD))
      {
        //Maye need to change the duration to 200.
        SetSpeedValuesDurationAndStop(-AUTO_REVERSE_SPEED, -AUTO_REVERSE_SPEED, REVERSE_DURATION_WALL);
        AddEncoderValues(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
        AddEncoderValuesIntoArray();
        break;
      }
      else
      {
        SetSpeedValuesDurationAndStop(-AUTO_REVERSE_SPEED, -AUTO_REVERSE_SPEED, REVERSE_DURATION);
        if (lineSensorValues[2] > QTR_THRESHOLD)
        {
          SetSpeedValuesDurationAndStop(-AUTO_REVERSE_SPEED, AUTO_REVERSE_SPEED, TURN_DURATION);
          AddEncoderValues(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
          delay(100);
        }
        else
        {
          SetSpeedValuesDurationAndStop(AUTO_REVERSE_SPEED, -AUTO_REVERSE_SPEED, TURN_DURATION);
          AddEncoderValues(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
          delay(100);
        }
      }
    }
  }
}

/*This forward movement function is slightly different to the other one this is for when we have pressed 'b'
  since we are not allowed to do anything else till we have either past the room. This will break out of the while
  loop when it has arrived at the T junction. It has line sensors to check if it's hit a wall at a angle and will
  recorrect itself.*/
void MovementForwardUsingDistance()
{
  turnSensorReset();
  SetEncodersAndSpeed(AUTO_FORWARD_SPEED, AUTO_FORWARD_SPEED);

  while (countsLeft + tempLeft < totalDistanceValueLeftEncoder + TJUNCTION && countsRight + tempRight < totalDistanceValueRightEncoder + TJUNCTION)
  {
   // delay(2);
    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsLeft - STRAIGHTFACTOR * countsRight;
    correction = Kp * error;
    currentSpeedRight = AUTO_FORWARD_SPEED + correction;
    SetSpeedValues(currentSpeedLeft, currentSpeedRight);
    lineSensors.read(lineSensorValues);

    if (lineSensorValues[2] > QTR_THRESHOLD || lineSensorValues[0] > QTR_THRESHOLD)
    {
      SetSpeedValuesDurationAndStop(-AUTO_REVERSE_SPEED, -AUTO_REVERSE_SPEED, REVERSE_DURATION);

      if (lineSensorValues[2] > QTR_THRESHOLD)
      {
        SetSpeedValuesDurationAndStop(-AUTO_REVERSE_SPEED, AUTO_REVERSE_SPEED, TURN_DURATION);
        AddEncodersToTempLeftAndTempRight(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
      }
      else
      {
        SetSpeedValuesDurationAndStop(AUTO_REVERSE_SPEED, -AUTO_REVERSE_SPEED, TURN_DURATION);
        AddEncodersToTempLeftAndTempRight(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
      }
      countsLeft = encoders.getCountsLeft();
      countsRight = encoders.getCountsRight();
    }
  }
}

/*Used for using the encoders to see how much to move into the room by
and then we use the same value to reverse out.*/ 
void MovementForwardSettingDistanceAndSpeed(int leftspeed, int rightspeed, int distance)
{
  turnSensorReset();
  SetEncodersAndSpeed(leftspeed, rightspeed);

  while (countsLeft < distance && countsRight < distance)
  {
    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
  }
  SpeedStop(); 
}

void MovementReverseSettingDistanceAndSpeed(int leftspeed, int rightspeed, int distance)
{
  turnSensorReset();
  SetEncodersAndSpeed(leftspeed, rightspeed);

  while (countsLeft > distance && countsRight > distance)
  {
    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
  }
  SpeedStop();
}

/*Same again very similiar to the other distance mvoements however this is called when we return home
  it uses the distances in the distance array to break out of the while loop. It uses sensors to check if
  it has hit a wall, if it has then if it's hit the wall with the middle sensor it will stop and break the while,
  if it hasn't then its hit a wall at a angle and will correct it's position.*/
void MovementForwardUsingDistanceAutomated()
{
  turnSensorReset();
  SetEncodersAndSpeed(AUTO_FORWARD_SPEED, AUTO_FORWARD_SPEED);

  while (countsLeft + tempLeft < leftDistanceEncoderArray[indexPositionDistance] && countsRight + tempRight < rightDistanceEncoderArray[indexPositionDistance])
  {
    countsLeft = encoders.getCountsLeft();
    countsRight = encoders.getCountsRight();
    error = countsLeft - STRAIGHTFACTOR * countsRight;
    correction = Kp * error;
    currentSpeedRight = AUTO_FORWARD_SPEED + correction;
    SetSpeedValues(currentSpeedLeft, currentSpeedRight);

    lineSensors.read(lineSensorValues);
    if (lineSensorValues[2] > QTR_THRESHOLD || lineSensorValues[0] > QTR_THRESHOLD)
    {
      delay(LINE_SENSOR_DELAY);
      lineSensors.read(lineSensorValues);

      if ((lineSensorValues[0] > QTR_THRESHOLD
           && lineSensorValues[1] > QTR_THRESHOLD)
          || (lineSensorValues[2] > QTR_THRESHOLD
              && lineSensorValues[1] > QTR_THRESHOLD))
      {
        SetSpeedValuesDurationAndStop(-AUTO_REVERSE_SPEED, -AUTO_REVERSE_SPEED, REVERSE_DURATION_WALL);
        break;
      }
      else
      {
        SetSpeedValuesDurationAndStop(-AUTO_REVERSE_SPEED, -AUTO_REVERSE_SPEED, REVERSE_DURATION);

        if (lineSensorValues[2] > QTR_THRESHOLD)
        {
          SetSpeedValuesDurationAndStop(-AUTO_REVERSE_SPEED, AUTO_REVERSE_SPEED, TURN_DURATION);
          AddEncodersToTempLeftAndTempRight(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
        }
        else
        {
          SetSpeedValuesDurationAndStop(AUTO_REVERSE_SPEED, -AUTO_REVERSE_SPEED, TURN_DURATION);
          AddEncodersToTempLeftAndTempRight(encoders.getCountsAndResetLeft(), encoders.getCountsAndResetRight());
        }
        countsLeft = encoders.getCountsLeft();
        countsRight = encoders.getCountsRight();
      }
    }
    SpeedStop();
  }
}

//Calibrate the line sensors for more accurate readings and precision, rotate 360 to get all the angles.
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

  AutomatedTurnLeftSpeed();
  while ((int32_t)turnAngle < turnAngle45 * 2)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }
  turnSensorReset();


  while ((int32_t)turnAngle < turnAngle45 * 2)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }

  turnSensorReset();

  while ((int32_t)turnAngle < turnAngle45 * 2)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }

  turnSensorReset();

  while ((int32_t)turnAngle < turnAngle45 * 2)
  {
    lineSensors.calibrate();
    turnSensorUpdate();
  }

  turnSensorReset();
  SpeedStop();
}
