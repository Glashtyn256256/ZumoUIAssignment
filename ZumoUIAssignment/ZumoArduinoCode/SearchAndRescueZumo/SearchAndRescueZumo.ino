#include <Zumo32U4.h>

//Serial1 communicates over XBee
//Serial communicates over USB cable

Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4LineSensors lineSensors;
Zumo32U4ProximitySensors proxSensors;

#define NUM_SENSORS 5
uint16_t lineSensorValues[NUM_SENSORS];
const uint16_t sensorThreshold = 200;
#define QTR_THRESHOLD     1000  // microseconds

// These might need to be tuned for different motor types.
#define REVERSE_SPEED     200  // 0 is stopped, 400 is full speed
#define TURN_SPEED        200
#define FORWARD_SPEED     200
#define REVERSE_DURATION  200  // ms
#define TURN_DURATION     300  // ms

void setup() {
  // put your setup code here, to run once:
  Serial1.begin(9600);
  Serial.begin(9600);
  lineSensors.initFiveSensors();
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

 if (lineSensorValues[0] < QTR_THRESHOLD)
  {
    // If leftmost sensor detects line, reverse and turn to the
    // right.
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }
  else if (lineSensorValues[NUM_SENSORS - 1] < QTR_THRESHOLD)
  {
    // If rightmost sensor detects line, reverse and turn to the
    // left.
    motors.setSpeeds(-REVERSE_SPEED, -REVERSE_SPEED);
    delay(REVERSE_DURATION);
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);
    delay(TURN_DURATION);
    motors.setSpeeds(FORWARD_SPEED, FORWARD_SPEED);
  }

printReadingsToSerial();


switch(incomingByte){
  case'w':  motors.setLeftSpeed(100);
            motors.setRightSpeed(100);
            //delay(2);
            break;
  
  case 'a': motors.setLeftSpeed(-100);
            motors.setRightSpeed(100);
            break;

case's':  motors.setLeftSpeed(-100);
            motors.setRightSpeed(-100);
            //delay(2);
            break;

 case 'd': motors.setLeftSpeed(100);
            motors.setRightSpeed(-100);
            break;
            
  case'e':  motors.setLeftSpeed(0);
            motors.setRightSpeed(0);
            break;
  
  }
}
