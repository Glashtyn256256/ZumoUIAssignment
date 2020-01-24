/* 
   SerialMessages.h and SerialMessages.cpp is where we store
   the messages for our code. I realised in my code that 
   the messages are repeated a lot so I condensed them down
   to save memory.
   
   Rememember and this is very important, Serial1 is what we
   use to communicate to the XBEE and Serial is what we use to
   communicate through the USB cable.
*/

#pragma once

#include <Zumo32U4.h>

void ManualMessage();
void AutomatedMessage();
void ReturningHomeMessage();
void AdjustMessage();

void MoveForwardMessage();
void MoveBackwardsMessage();
void TurnLeftMessage();
void TurnRightMessage();
void TurnLeftDegreesMessage();
void TurnRightDegreesMessage();
void TurningCompletedMessage();
void PressToContinueMessage();
void TurningHalfDegreesMessage();
void MovementStoppedMessage();

void ArrivedTJunctionMessage();

void SearchRoomMessage();
void SurvivorMessage(int roomnumber);
void RoomEmptyMessage(int roomnumber);
void SendSignalMessage();
void SurvivorLeftMessage();

void ReturnedBackToHomeBaseMessage();
