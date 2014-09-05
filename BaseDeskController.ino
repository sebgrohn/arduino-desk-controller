
#include "BaseDeskController.h"


BaseDeskController::BaseDeskController(const int& upPin, const int& downPin)
    : upPin(upPin), downPin(downPin) {
  pinMode(upPin, OUTPUT);
  pinMode(downPin, OUTPUT);
  
  enabled = true;
}

boolean BaseDeskController::isEnabled() const { return enabled; }

DeskDrivingDirection BaseDeskController::getDrivingDirection() const {return drivingDirection; }

boolean BaseDeskController::isDriving() const { return drivingDirection != NONE; }

boolean BaseDeskController::isDrivingUp() const { return drivingDirection == UP; }

boolean BaseDeskController::isDrivingDown() const { return drivingDirection == DOWN; }

void BaseDeskController::setEnabled(const boolean& newEnabled) {
  if (!newEnabled) {
    stopDrive();
  }
  
  enabled = newEnabled;
}

void BaseDeskController::startDrive(const DeskDrivingDirection& direction) {
  if (!enabled) {
    return;
  }
  
  switch (direction) {
  case UP:
    drivingDirection = UP;
    digitalWrite(downPin, LOW);
    digitalWrite(upPin, HIGH);
    break;
    
  case DOWN:
    drivingDirection = DOWN;
    digitalWrite(upPin, LOW);
    digitalWrite(downPin, HIGH);
    break;
  }
}

void BaseDeskController::stopDrive() {
  digitalWrite(upPin, LOW);
  digitalWrite(downPin, LOW);
  drivingDirection = NONE;
}

