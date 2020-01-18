//#define ENCODER_TURN_LIMIT 658

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//void printReadingsToSerial()
//{
//  char buffer[80];
//  sprintf(buffer, "%4d %4d %4d %c\n",
//          lineSensorValues[0],
//          lineSensorValues[1],
//          lineSensorValues[2]
//         );
//  Serial.print(buffer);
//}

//void printReadingsToSerials()
//{
//  char buffer[80];
//  sprintf(buffer, "%1d %1d %c\n",
//          leftValue, rightValue
//         );
//  Serial1.print(buffer);
//}
//--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//void TurnLeftUsingEncoders()
//{
//  MotorSpeedTurnLeft();
//  int error;
//  int correction;
//  int currentSpeedLeft = TURN_SPEED;
//  int currentSpeedRight = TURN_SPEED;
//
//  encoders.getCountsAndResetLeft();
//  encoders.getCountsAndResetRight();
//  int countsLeft =  encoders.getCountsLeft();
//  int countsRight = encoders.getCountsRight();
//
//  while ((countsLeft < ENCODER_TURN_LIMIT) && (countsRight < ENCODER_TURN_LIMIT))
//  {
//
//    countsLeft = -encoders.getCountsLeft();
//    countsRight = encoders.getCountsRight();
//    error = countsLeft - STRAIGHTFACTOR * countsRight;
//    correction = Kp * error;
//    currentSpeedRight = TURN_SPEED + correction;
//    motors.setRightSpeed(currentSpeedRight);
//  }
//        if ((countsLeft < ENCODER_TURN_LIMIT))
//        {
//          while ((countsLeft < ENCODER_TURN_LIMIT))
//          {
//            countsLeft = -encoders.getCountsLeft();
//            countsRight = encoders.getCountsRight();
//            motors.setRightSpeed(-100);
//
//          }
//        }
//        else if (countsRight < ENCODER_TURN_LIMIT)
//        {
//          while ((countsRight < ENCODER_TURN_LIMIT))
//          {
//            countsLeft = -encoders.getCountsLeft();
//            countsRight = encoders.getCountsRight();
//            motors.setLeftSpeed(100);
//
//          }
//        }
//  motors.setLeftSpeed(0);
//  motors.setRightSpeed(0);
//  //reset encoders, we only want the encoders for when it's going straight.
//  Serial1.println(encoders.getCountsAndResetLeft());
//  Serial1.println(encoders.getCountsAndResetRight());
//  turnSensorReset();
//}

//void TurnRightUsingEncoders()
//{
//  MotorSpeedTurnRight();
//  int error;
//  int correction;
//  int currentSpeedLeft = TURN_SPEED;
//  int currentSpeedRight = TURN_SPEED;
//
//  encoders.getCountsAndResetLeft();
//  encoders.getCountsAndResetRight();
//  int countsLeft =  encoders.getCountsLeft();
//  int countsRight = encoders.getCountsRight();
//  while ((countsRight < ENCODER_TURN_LIMIT) && (countsLeft < ENCODER_TURN_LIMIT))
//  {
//    countsLeft = encoders.getCountsLeft();
//    countsRight = -encoders.getCountsRight();
//    error = countsRight - STRAIGHTFACTOR * countsLeft;
//    correction = Kp * error;
//    currentSpeedLeft = TURN_SPEED + correction;
//    motors.setLeftSpeed(currentSpeedLeft);
//  }
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
//  motors.setLeftSpeed(0);
//  motors.setRightSpeed(0);
//  //reset encoders, we only want the encoders for when it's going straight.
//  //Serial1.println(encoders.getCountsAndResetLeft());
//  encoders.getCountsAndResetLeft();
//  encoders.getCountsAndResetRight();
//  //Serial1.println(encoders.getCountsAndResetRight());
//  turnSensorReset();
//}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/***
    Was used for when I wanted to search a room, I would set the amount I want
    the encoders to get to, by passing in the number through the parameter. Since
    the room is small enough that I can scan without havint to move into and got
    confirmation I didn't have to have it. I removed it, however if the room was
    bigger then I would need to use this.
***/
//void MovementForwardUsingDistance(int distance)
//{
//  turnSensorReset();
//  int error;
//  int correction;
//  int currentSpeedLeft = MAIN_SPEED;
//  int currentSpeedRight = MAIN_SPEED;
//  encoders.getCountsAndResetLeft();
//  encoders.getCountsAndResetRight();
//  int countsLeft =  encoders.getCountsLeft();
//  int countsRight = encoders.getCountsRight();
//
//  motors.setSpeeds(MAIN_SPEED, MAIN_SPEED);
//  while (countsLeft < distance && countsRight < distance)
//  {
//    delay(2);
//    countsLeft = encoders.getCountsLeft();
//    countsRight = encoders.getCountsRight();
//    error = countsLeft - STRAIGHTFACTOR * countsRight;
//    correction = Kp * error;
//    currentSpeedRight = MAIN_SPEED + correction;
//    motors.setSpeeds(currentSpeedLeft, currentSpeedRight);
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
//  }
//}

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//bool ScanRoomProximityTurnRightEncoder(bool objectseen)
//{
//  MotorSpeedTurnRight();
//  turnSensorReset();
//  int error;
//  int correction;
//  int currentSpeedLeft = TURN_SPEED;
//  int currentSpeedRight = TURN_SPEED;
//
//  encoders.getCountsAndResetLeft();
//  encoders.getCountsAndResetRight();
//  int countsLeft =  encoders.getCountsLeft();
//  int countsRight = encoders.getCountsRight();
//  do {
//    delay(1);
//    turnSensorUpdate();
//    countsLeft =  encoders.getCountsLeft();
//    countsRight = -encoders.getCountsRight();
//    error = countsLeft - STRAIGHTFACTOR * countsRight;
//    correction = Kp * error;
//    currentSpeedRight = TURN_SPEED + correction;
//    motors.setRightSpeed(currentSpeedRight);
//
//    if (objectseen == false)
//    {
//      proxSensors.read();
//      leftValue = proxSensors.countsFrontWithLeftLeds();
//      rightValue = proxSensors.countsFrontWithRightLeds();
//      // printReadingsToSerials();
//      objectseen = leftValue >= sensorProximityThreshold || rightValue >= sensorProximityThreshold;
//    }
//
//  } while ((countsRight < ENCODER_TURN_LIMIT) && (countsLeft < ENCODER_TURN_LIMIT));
//  MotorSpeedStop();
//  turnSensorReset();
//  return objectseen;
//}

//bool ScanRoomProximityTurnLeftEncoder(bool objectseen)
//{
//  MotorSpeedTurnLeft();
//  turnSensorReset();
//  int error;
//  int correction;
//  int currentSpeedLeft = TURN_SPEED;
//  int currentSpeedRight = TURN_SPEED;
//
//  encoders.getCountsAndResetLeft();
//  encoders.getCountsAndResetRight();
//  int countsLeft =  encoders.getCountsLeft();
//  int countsRight = encoders.getCountsRight();
//  do {
//    delay(1);
//    turnSensorUpdate();
//    countsLeft = -encoders.getCountsLeft();
//    countsRight = encoders.getCountsRight();
//    error = countsLeft - STRAIGHTFACTOR * countsRight;
//    correction = Kp * error;
//    currentSpeedRight = TURN_SPEED + correction;
//    motors.setRightSpeed(currentSpeedRight);
//
//    if (objectseen == false)
//    {
//      proxSensors.read();
//      leftValue = proxSensors.countsFrontWithLeftLeds();
//      rightValue = proxSensors.countsFrontWithRightLeds();
//      // printReadingsToSerials();
//      objectseen = leftValue >= sensorProximityThreshold || rightValue >= sensorProximityThreshold;
//    }
//  } while ((countsLeft < ENCODER_TURN_LIMIT) && (countsRight < ENCODER_TURN_LIMIT));
//  MotorSpeedStop();
//  turnSensorReset();
//  return objectseen;
//}
