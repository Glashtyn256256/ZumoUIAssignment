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
  Serial1.println("Manual: ");
}
void AutomatedMessage()
{
  Serial1.println("Automated: ");
}
void ReturningHomeMessage() {
  Serial1.println("Returning Home: ");
}
void AdjustMessage(){
  Serial1.println("Adjust: ");
}

void MoveForwardMessage()
{
  Serial1.print("Moving Forward");
}
void MoveBackwardsMessage()
{
  Serial1.print("Moving Backwards");
}
void TurnLeftMessage()
{
  Serial1.print("Turning Left");
}
void TurnRightMessage() {
  Serial1.print("Turning Right");
}

void TurnLeftDegreesMessage(){
  Serial1.print("Turning Left 90 Degrees");
}
void TurnRightDegreesMessage()
{
  Serial1.print("Turning Right 90 Degrees");
}

void TurningCompletedMessage(){
  Serial1.print("Turning Completed");
}
void PressToContinueMessage(){
  Serial1.println("Press C Or Start Automation To Move Forward");
}
void TurningHalfDegreesMessage(){
Serial1.print("Turning 180 Degrees");
}
void MovementStoppedMessage(){
  Serial1.println("Movement Stopped");
}

void ArrivedTJunctionMessage(){
  Serial1.print("We've Arrived At The T Junction");
}

void SearchRoomMessage(){
  Serial1.print("Searching Room For Survivor");
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
  Serial1.print("Zumo Has Returned To Home Base");
}
