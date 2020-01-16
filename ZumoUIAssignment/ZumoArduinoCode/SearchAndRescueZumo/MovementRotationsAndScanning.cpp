/* MovementRotationsAndScanning.h and MovementRotationsAndScanning.cpp 
 *  contains all our functions that we call to allow us to move around
 *  the map. This can be moving, rotating or scanning rooms.
 * 
 * This was the most diffcult part of this project due to the motors 
 * being out of sync in power. One motor is more powerful than the other. 
 * This meant that if you went in a straght line it would start to curve 
 * into a wall and the rotations were unreliable. 
 * 
 * I orginally used just the motorspeeds and delays to go across the map
 * this worked but was insonsient and was prone to getting stuck on walls 
 * or being stuck in the corner of a corrdior. It was also not very accurate
 * so I had to find another alternative
 * 
 * I tried numerous attempts in hardcoding the motor that had less power 
 * however again it was unreliable.I came across another method that adjusted 
 * the speed according to what angle we are currently, we would get this angle from the gyro. 
 * https://github.com/pvcraven/zumo_32u4_examples/blob/master/HeadingHold/HeadingHold.ino
 * More information on it here. However it was still slightly curving and I wanted to to be 
 * as straight as it can be I looked again to see what I could find and
 * 
 *  I found a method that used the encoders and it was the method that I have used in all of 
 *  the movement and rotation functions. Using the encoders to work out the correction,
 *  depending which one you want want to compensate for. Example if it's the right.
 *  You would mutliply the right encoder value with a number between 0.9 to 1.1, this is to adjust for any 
 *  minor curves, I believe mine is 1.015. You then take this value off the left encoder to give us our 
 *  correction, you then add this correction to the current speed of the motors. Then add the final result
 *  to the speed of the right motor. This will apply the correct compensation to keep us in a straight line.
 *  This has worked very well for me and so I've used it.
 *  http://www.abrowndesign.com/2017/02/25/zumo-32u4-synchronize-motor/
 * This was the handy guide that helped me correct my motor issues. I applied
 * this also to my rotations which works most the time. I believe the reason 
 * the rotations isn't always accurate is due to the gyro.
 * 
 * I also had to make sure the zumo would stop when it hits the wall
 * this was when we used the linesensors to detect if it was over a line
 * or not. Luckily I didnt have to much of a issue with this thanks to
 * following the Maze Solver in the zumo32u4 examples. This allowed us to 
 * calibrate the line sensors allowing for more precsion for the sensors when
 * it goes over a line. I did have to add a delay though once it hit a right
 * or left sensor since the middle sensor is further back so it needs time to 
 * get into positon. This allows us to know if we hit a wall straight or if we 
 * hit the wall at an angle.
 * 
 * Rotations are fairly straight forward we used the Gyro to workout what angle we
 * are at we need to make sure we update the angle each loop so the gyro knows the 
 * how much it has turned by. You then want to make sure you reset the gyro once 
 * you're finished or your gyro data will be wrong.
 * https://github.com/pvcraven/zumo_32u4_examples/blob/master/TurnExample/TurnExample.ino
 * ^I used this example in my code, I did use the Maze Solver example but found this one better.
 * I had tested rotating by just using encoders which is commented out in my code somewhere and delays
 * but again the same issue they we're not accurate enough and the Gyro was the one that
 * proved more reliable but still isn't 100%. I actually applied the encoder speed correction
 * to the Gyro to compensate for the power difference in the motors that had improved the accuracy
 * by a small margin.
 * 
 * 
*/

#include <Wire.h>
#include "MovementRotationsAndScanning.h"
