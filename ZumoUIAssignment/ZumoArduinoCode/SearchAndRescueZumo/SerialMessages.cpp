/* SerialMessages.h and SerialMessages.cpp is where we store
    the messages for our code. I realised in my code that
    the messages are repeated a lot so I condensed them down
    to save memory. (Dynamic Memory is now down to 54%. it  
    was originally 86%, definitly was a good choice doing this)

    Rememember and this is very important, Serial1 is what we
    use to communicate to the XBEE and Serial is what we use to
    communicate through the USB cable.
*/

#include <Wire.h>
#include "SerialMessages.h"

void ManualMessage() {
  Serial1.print("Manual: ");
}
void AutomatedMessage()
{
  Serial1.print("Automated: ");
}
void ReturningHomeMessage() {
  Serial1.print("Returning Home: ");
}
void AdjustMessage(){
  Serial1.print("Adjust: ");
}

void MoveForwardMessage()
{
  Serial1.println("Moving Forward");
}
void MoveBackwardsMessage()
{
  Serial1.println("Moving Backwards");
}
void TurnLeftMessage()
{
  Serial1.println("Turning Left");
}
void TurnRightMessage() {
  Serial1.println("Turning Right");
}

void TurnLeftDegreesMessage(){
  Serial1.println("Turning Left 90 Degrees");
}
void TurnRightDegreesMessage()
{
  Serial1.println("Turning Right 90 Degrees");
}

void TurningCompletedMessage(){
  Serial1.println("Turning Completed");
}
void PressToContinueMessage(){
  Serial1.println("Press C Or Start Automation To Move Forward");
}
void TurningHalfDegreesMessage(){
Serial1.println("Turning 180 Degrees");
}
void MovementStoppedMessage(){
  Serial1.println("Movement Stopped");
}

void ArrivedTJunctionMessage(){
  Serial1.println("We've Arrived At The T Junction");
}

void SearchRoomMessage(){
  Serial1.println("Searching Room For Survivor");
}

void SurvivorMessage(int roomnumber)
{
  Serial1.print("#Room ");
  Serial1.print(roomnumber);
  Serial1.println(" Searched: Survivor Inside !");
}

void RoomEmptyMessage(int roomnumber)
{
  Serial1.print("#Room ");
  Serial1.print(roomnumber);
  Serial1.println(" Searched: Room Empty !");
}

void SendSignalMessage() {
  Serial1.println("Searched: Survivor Inside Sending Signal! ");
}

void SurvivorLeftMessage() {
  Serial1.println("Searched: Survivor Has Left The Room");
}

void ReturnedBackToHomeBaseMessage()
{
  Serial1.println("Zumo Has Returned To Home Base");
}
